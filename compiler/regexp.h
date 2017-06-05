#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
#include <fstream>

#include "operator.h" // 添加符号支持
#include "NFA.h"
#include "DFA.h"

using namespace std;

map<char, int> preprocess() {
	map<char, int> m;
	m['*'] = CLOUSRE;
	m[link_operator] = LINK; // '·' 和 "·" 编码竟然完全不一样，不得已，用其他符号替代
	m['|'] = OR;
	return m;
}

bool is_digit(char c) {
	return c >= '0'&&c <= '9';
}

bool is_alpha(char c) {
	return c >= 'A'&&c <= 'Z' || c >= 'a'&&c <= 'z';
}

namespace reg2NFA {

	// 中缀转后缀
	string infix2postfix(string &infix) {
		map<char, int> lookup_table = preprocess();
		stack<char> op;
		string postfix;
		for (auto ch : infix) {
			if (is_alpha(ch) || is_digit(ch))postfix += ch;
			else {
				//ch == '(' || op.top() == '(' 是两件事: 没入栈的左括号优先级最高，入栈后优先级最低
				if (op.empty() || ch == '(' || op.top() == '(' || lookup_table[ch] > lookup_table[op.top()]) op.push(ch);
				else if (ch == ')') {
					while (!op.empty() && op.top() != '(') postfix += op.top(), op.pop();
					try
					{
						if (op.empty())throw string("正规式的右括号多于左括号，请检查正规式");
					}
					catch (string& err)
					{
						cout << endl << "Error: ***" << err << "***" << endl << endl;
						exit(1);
					}
					op.pop(); // 弹出左括号
				}
				else {
					// 这里注意, 有括号情况下, 左括号只能和右括号匹配, 因此，不能提前弹出左括号 op.top() != '('
					while (!op.empty() && op.top() != '(' && lookup_table[ch] <= lookup_table[op.top()]) postfix += op.top(), op.pop();
					op.push(ch);
				}
			}
		}
		while (!op.empty()) {
			char top = op.top(); op.pop();
			try {
				if (top == '(')throw string("正规式的左括号多于右括号，请检查正规式");
			}
			catch (string& err) {
				cout << endl << "Error: ***" << err << "***" << endl << endl;
				exit(1);
			}
			postfix += top;
		}
		return postfix;
	}

	// 为正规式添加连接运算符
	string add_link_op(string &reg) {
		string regexp = reg;
		for (int cur = 0, next = 1; next < regexp.length(); cur++, next++) {
			char c = regexp[cur], n = regexp[next];
			if (n == '(' || is_alpha(n))
				if (c == ')' || c == '*' || is_alpha(c))
					regexp.insert(next, link_op_str);
		}
		return regexp;
	}

	NFA* regexp2NFA(string& regexp) {
		// 预处理: 添加连接符, 转化为后缀形式
		string& t_str = add_link_op(regexp);
		string& reg = infix2postfix(t_str);
		stack<NFA*> s;
		try
		{
			for (auto ch : reg) {
				if (is_alpha(ch) || is_digit(ch)) {
					NFA* nfa = new NFA(ch);
					s.push(nfa);
				}
				else if (op_set.count(ch)) {
					NFA *back_nfa = s.top(); s.pop();
					NFA *front_nfa = nullptr;
					switch (ch)
					{
					case '-':
						front_nfa = s.top(); s.pop();
						NFA *connect_nfa;
						connect_nfa = (*front_nfa) + (*back_nfa);
						s.push(connect_nfa);
						break;
					case '|':
						front_nfa = s.top(); s.pop();
						NFA *or_nfa;
						or_nfa = (*front_nfa) | (*back_nfa);
						s.push(or_nfa);
						break;
					case '*':
						NFA *closure;
						closure = ++(*back_nfa);
						s.push(closure);
						break;
					default:
						throw (string("从 regexp.h 文件定义的 regexp2NFA 退出。检测到未知的符号:") + ch);
					}
				}
				else throw (string("正规式中检测到非法字符: ") + ch);
			}
		}
		catch (string& err)
		{
			cout << "Error:***" << err << "***" << endl;
			exit(2);
		}
		NFA *nfa_ptr = s.top();
		s.pop();
		nfa_ptr->init_start_end_node();
		nfa_ptr->set_regexp(regexp);
		return nfa_ptr;
	}
}

namespace NFA2DFA {

	set<char> * get_alpha_table(string &reg) {
		set<char> *alpha_set = new set<char>;
		for (auto ch : reg)
			if (is_alpha(ch) || is_digit(ch))
				alpha_set->insert(ch);
		return alpha_set;
	}

	// 将访问到的 NFA 结点加入队列和集合, 一步转移的辅助函数, 只将ε边的结点加入队列遍历
	void vis_NFA_nodes(N_node *cur_node, char ch, queue<N_node*> &bfs, set<N_node*> &vis) {
		vector<N_node*> *next_nodes = cur_node->next[ch];
		for (auto n : *next_nodes) {
			if (vis.count(n))continue;
			vis.insert(n);
			bfs.push(n);
		}
	}

	// 将集合中的NFA结点压入队列, 辅助函数
	void push_NFA_node_2_queue(queue<N_node*> &q, set<N_node*> &s) {
		for (auto n : s)q.push(n);
	}

	// 一步转移, ε边可以走多步, 但是正常字符边只能走一步, 不能做dfs或者bfs
	set<N_node*> *move(set<N_node*> *cur, char ch) {
		queue<N_node*> bfs;
		set<N_node*> vis;// vis作用2个: 1. bfs 查重  2. 记录move过程中的结点, 最终vis得到的就是本次的次态结点集
		set<N_node*> *node_set = nullptr;
		bool init = false;
		// 一步转移
		for (auto node : *cur) {
			if (node->has_char(ch)) {
				if (init == false)init = true;
				vis_NFA_nodes(node, ch, bfs, vis);
			}
		}
		// 求ε闭包
		while(!bfs.empty()) {
			N_node *cur_node = bfs.front(); bfs.pop();
			if (cur_node->has_char(epsilon)) {
				vis_NFA_nodes(cur_node, epsilon, bfs, vis);
			}
		}
		if (init) {
			node_set = new set<N_node *>;
			for (auto node : vis)node_set->insert(node);
		}
		return node_set;
	}

	// 构造 起始 DFA 结点
	D_node *generate_start_node(int node_id, NFA *nfa) {
		const int status = (1 << DFA_space::START);
		D_node *new_dnode = nullptr;
		set<N_node*> *node_set = new set<N_node *>;
		node_set->insert(nfa->get_start());
		// 通过遍历ε变获得起始结点集
		set<N_node*> *n_ptrs = move(node_set, epsilon);
		// 若结点集合为 nullptr, 则说明本结点就是起始结点, 反之, 起始结点就应和这些节点合并
		if (n_ptrs != nullptr) {
			for (auto node : *n_ptrs) node_set->insert(node);
			n_ptrs->clear();
			delete n_ptrs;
		}
		// 创建起始结点
		new_dnode = new D_node(node_id, status, node_set);
		return new_dnode;
	}

	int get_DFA_node_state(set<N_node*> *s) {
		for (auto node : *s) {
			if (node->is_end())return (1 << DFA_space::END);
		}
		return (1 << DFA_space::MID);
	}

	DFA* nfa2dfa(NFA *nfa) {
		if (nfa == nullptr || nfa->get_start() == nullptr)return nullptr;

		// 如果我们需要通过一步转移来计算每个DFA结点的NFA结点集合，那么我们需要通过一个个结点集合来构造DFA
		DFA *new_dfa = new DFA;
		// 获取字母集
		set<char> *alpha_set = get_alpha_table(nfa->get_regexp());
		new_dfa->set_alpha_table(alpha_set);
		// 首先我们走 ε边 得到起始结点
		D_node *start = generate_start_node(new_dfa->get_next_id(), nfa);
		new_dfa->update_next_id(); // 更新 next id
		// 对于闭包情况必须进行预处理, 因为启示结点可能同时也是终结节点
		start->status |= get_DFA_node_state(start->NFA_nodes);
		new_dfa->set_start_node(start);

		// 开始一步转移
		queue<D_node*> bfs;
		bfs.push(start);
		new_dfa->add_nfa_node_set(start->NFA_nodes);// 结点查重
		while(!bfs.empty()) {
			D_node *cur = bfs.front(); bfs.pop();
			// 遍历可能的字符
			for (auto ch : *alpha_set) {
				set<N_node*> *next = move(cur->NFA_nodes, ch);
				// 没有当前字符
				if (next == nullptr)continue;
				// 若重复, 说明形成闭包, 更新链表同时释放内存, 释放内存是因为next中存储的内容已经存储过了
				// 而且如果不释放, 将会导致内存泄漏
				if (new_dfa->has_nfa_node_set(next)) {
					D_node * dptr = new_dfa->get_D_node_addr_by_set(next);
					cur->add_next(ch, dptr);
					delete next;
				}
				// 新的结点, 加入队列, 增加映射关系
				else {
					new_dfa->add_nfa_node_set(next); // 查重
					D_node *new_dn = new D_node(new_dfa->get_next_id(), get_DFA_node_state(next), next);
					new_dfa->update_next_id();
					cur->add_next(ch, new_dn);
					bfs.push(new_dn);
					new_dfa->add_map_set2NFA_node_addr(next, new_dn);// 增加映射关系
				}
			}
		}
		new_dfa->set_regexp(nfa->get_regexp());
		return new_dfa;
	}
}
