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

set<char> special = { '.','`','^','\"' };

bool is_digit(char c) {
	return c >= '0'&&c <= '9';
}

bool is_alpha(char c) {
	return c >= 'A'&&c <= 'Z' || c >= 'a'&&c <= 'z' || special.count(c);
}

namespace reg2NFA {

	// 中缀转后缀
	string infix2postfix(string &infix) {
		map<char, int> lookup_table = preprocess();
		stack<char> op;
		string postfix;
		for (auto ch : infix) {
			if (is_alpha(ch) || is_digit(ch)||special.count(ch))postfix += ch;
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
			if (n == '(' || is_alpha(n)||special.count(n))
				if (c == ')' || c == '*' || is_alpha(c))
					regexp.insert(next, link_op_str);
		}
		return regexp;
	}

	NFA* regexp2NFA(string& regexp) {
		// 预处理: 添加连接符, 转化为后缀形式
		cout << "正在将正规式转化为NFA, 请稍后..." << endl;
		string& t_str = add_link_op(regexp);
		string& reg = infix2postfix(t_str);
		stack<NFA*> s;
		try
		{
			for (auto ch : reg) {
				if (is_alpha(ch) || is_digit(ch)|| special.count(ch)) {
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
		cout << "正规式转化为NFA完成!" << endl;
		return nfa_ptr;
	}
}

namespace NFA2DFA {

	//////////////////////////////////////////////////////////////////////////
	
	// NFA转DFA

	set<char> * get_alpha_table(string &reg) {
		set<char> *alpha_set = new set<char>;
		for (auto ch : reg)
			if (is_alpha(ch) || is_digit(ch)||special.count(ch))
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

	// 获得DFA结点状态, NFA转DFA辅助函数
	int get_DFA_node_state(set<N_node*> *s) {
		for (auto node : *s) {
			if (node->is_end())return (1 << DFA_space::END);
		}
		return (1 << DFA_space::MID);
	}

	DFA* nfa2dfa(NFA *nfa) {
		if (nfa == nullptr || nfa->get_start() == nullptr)return nullptr;
		cout << "正在将NFA转化为DFA，请稍后..." << endl;

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
		cout << "NFA转化为DFA完成!" << endl;
		return new_dfa;
	}

	//////////////////////////////////////////////////////////////////////////

	// DFA 化简

	// 将DFA结点分为终结组和非终结组, DFA化简的辅助函数
	void divide_D_nodes_2_end_not_end(DFA *dfa, stack<set<D_node*>*> &nodes_stack, set<set<D_node*>> &cur_state) {
		queue<D_node*> bfs;
		set<D_node*> vis;

		set<D_node*> *end = new set<D_node *>, *not_end = new set<D_node *>;// 终结组和非终结组

		bfs.push(dfa->get_start_node());
		vis.insert(dfa->get_start_node());
		while (!bfs.empty()){
			D_node *node = bfs.front(); bfs.pop();
			if (node->is_end())end->insert(node);
			else not_end->insert(node);
			for (auto p : node->next) {
				if (vis.count(p.second))continue;
				vis.insert(p.second);
				bfs.push(p.second);
			}
		}
		nodes_stack.push(end); nodes_stack.push(not_end);
		cur_state.insert(*end); cur_state.insert(*not_end);
	}

	// 维护DFA结点地址和下标的映射关系, DFA化简的辅助函数
	void modify_addr_2_index(map<D_node*, int> &m, set<D_node*>* s, int index) {
		if (index < 0) {
			cout << "modify_addr_2_index 方法中index小于0, 请检查index参数" << endl;
			return;
		}
		for (auto addr : *s)m[addr] = index;
	}

	// 构建新的最简DFA, DFA化简的辅助函数
	D_node* generate_DFA_nodes_by_vec(vector<set<D_node*>*> &vec, map<D_node*, int> &addr_2_index, DFA *dfa) {
		set<D_node*> vis;
		set<D_node*> vis_id; // 防止重复设置id
		queue<D_node*> bfs;
		D_node *cur_start = dfa->get_start_node();

		// 用vector暂存新DFA结点指针
		// 可以避免函数返回时, 数组内存泄漏或析构数组导致 D_node 被析构
		vector<D_node*> temp;
		// 创建DFA结点备用
		for (auto nodes: vec) {
			int status = 0;
			for (auto node : *nodes)status |= node->status;
			temp.push_back(new D_node(0, status, nullptr));
		}
		// 遍历原DFA, 根据各条边的关系和地址到结点下标的映射, 建立新的DFA的边
		vis.insert(cur_start);
		bfs.push(cur_start);
		dfa->init_next_id();// 重置next_id属性
		// 开始 bfs
		while (!bfs.empty()) {
			D_node *node = bfs.front(); bfs.pop();
			// 根据原关系添加新结点间的关系
			D_node *new_cur = temp[addr_2_index[node]]; // node 是旧结点, new_cur 是其对应的新结点
			// 防止重复设置id
			if (!vis_id.count(new_cur)) {
				vis_id.insert(new_cur);
				new_cur->id = dfa->get_next_id(), dfa->update_next_id();
			}
			for (auto p : node->next) {
				new_cur->add_next(p.first, temp[addr_2_index[p.second]]);// temp[addr_2_index[p.second]] 是新的次态结点
				// 原结点加入bfs队列
				if (!vis.count(p.second)) {
					vis.insert(p.second);
					bfs.push(p.second);
				}
			}
		}
		return temp[addr_2_index[cur_start]];
	}

	// 添加已经简化的新DFA结点并维护状态表, 化简DFA的辅助函数
	void add_new_node(vector<set<D_node*>*> &res_nodes_addr, map<D_node*, int> &addr_2_index, set<D_node*> *nodes) {
		res_nodes_addr.push_back(nodes);
		modify_addr_2_index(addr_2_index, nodes, res_nodes_addr.size() - 1);
	}

	// 化简DFA
	void simplify_DFA(DFA *dfa) {
		if (dfa == nullptr || dfa->get_start_node() == nullptr) {
			cout << "DFA为 nullptr 请检查传入的 DFA" << endl;
			return;
		}
		cout << "正在化简DFA，请稍后..." << endl;
		dfa->set_simplyfied(true);

		set<set<D_node*>> cur_groups;// 当前分组
		map<D_node*, int> addr_2_index; // 记录未化简的DFA结点地址到化简后的DFA结点数组的下标对应关系
		stack<set<D_node*>*> nodes_stack;
		vector<set<D_node*>*> res_nodes_addr; // 存储最终结果的可变数组, set中存放的是未化简的DFA结点的地址

		// 将结点分为终结结点和非终结结点, 并加入nodes_stack, cur_group中
		divide_D_nodes_2_end_not_end(dfa, nodes_stack, cur_groups);
		// 构建结点分组划分
		while (!nodes_stack.empty()) {
			set<D_node*> *nodes = nodes_stack.top(); nodes_stack.pop();
			// 若划分存在空集, 如闭包 a* 的非终结符集则释放内存并直接下一步
			if (nodes->empty()) {
				delete nodes;
				continue;
			}
			if (nodes->size() == 1) {
				add_new_node(res_nodes_addr, addr_2_index, nodes);// 产生新结点, 维护关系
				continue;
			}

			map<set<D_node*>, vector<D_node*>> set2nodes; // key: 存储各个集合划分的指针, value: 属于 key 集合的结点地址
			bool should_divide = false;

			for (auto ch : *dfa->get_alpha_table()) {
				set2nodes.clear();
				set<D_node*> null_set; //  记录给定边未nullptr的结点集合
				// 用字符 ch 遍历当前集合所有结点
				for (auto node : *nodes) {
					// 所有没有对应的边的当前结点, 也需要从当前集合中分离出来
					if (!node->has_next(ch)) {
						
						if (!set2nodes.count(null_set))set2nodes[null_set] = vector<D_node*>();
						set2nodes[null_set].push_back(node);
						continue;
					}
					// 反之, 如果存在ch边, 则将包含次态结点的组的地址加入 set2nodes 中, 如果遍历一个字符后,
					// set2nodes 包含的组数目超过一个, 则说明分支超过一个, 需要划分
					for (auto group : cur_groups) {
						if (group.count(node->get_next_node(ch))) {
							if (!set2nodes.count(group)) set2nodes[group] = vector<D_node*>();// 这样是可以的
							set2nodes[group].push_back(node);
							break;
						}
					}
				}
				// 如果当前结点的分支超过1个, 说明需要划分, 具体的分支信息存储在 set2nodes 中
				if (set2nodes.size() > 1) {
					should_divide = true;
					break;
				}
			}
			// 如果需要划分, 则利用set2nodes中的信息将当前分组拆成若干新分组
			// 将新分组加入cur_groups, 并删除旧结点, 释放内存
			if (should_divide) {
				for (auto p : set2nodes) {
					set<D_node*> *new_group = new set<D_node *>;
					for (auto node_ptr : p.second)new_group->insert(node_ptr);
					// 更新 cur_groups, 压入栈中
					cur_groups.insert(*new_group);
					nodes_stack.push(new_group);
				}
				// 删除旧集合, 释放内存
				cur_groups.erase(cur_groups.find(*nodes));
				delete nodes;
			}
			// 如果不需要划分, 说明当前结点已经是化简后的结点, 放入最终的vector中待用
			// 注意对结点地址到下标这一映射关系的维护
			else add_new_node(res_nodes_addr, addr_2_index, nodes);// 产生新结点, 维护关系
		}
		// 从 vector 中获取节点并构建最简DFA
		D_node* new_start = generate_DFA_nodes_by_vec(res_nodes_addr, addr_2_index, dfa);
		// 构建完成后释放内存, 重新设置DFA的起始结点
		for (auto ptr : res_nodes_addr)delete ptr;
		dfa->destroy_cur_dfa_nodes();
		dfa->set_start_node(new_start);

		cout << "化简DFA完成!" << endl;

	}
}
