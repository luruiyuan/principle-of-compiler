#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
#include <fstream>

#include "operator.h" // ��ӷ���֧��
#include "NFA.h"
#include "DFA.h"

using namespace std;

map<char, int> preprocess() {
	map<char, int> m;
	m['*'] = CLOUSRE;
	m[link_operator] = LINK; // '��' �� "��" ���뾹Ȼ��ȫ��һ���������ѣ��������������
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

	// ��׺ת��׺
	string infix2postfix(string &infix) {
		map<char, int> lookup_table = preprocess();
		stack<char> op;
		string postfix;
		for (auto ch : infix) {
			if (is_alpha(ch) || is_digit(ch))postfix += ch;
			else {
				//ch == '(' || op.top() == '(' ��������: û��ջ�����������ȼ���ߣ���ջ�����ȼ����
				if (op.empty() || ch == '(' || op.top() == '(' || lookup_table[ch] > lookup_table[op.top()]) op.push(ch);
				else if (ch == ')') {
					while (!op.empty() && op.top() != '(') postfix += op.top(), op.pop();
					try
					{
						if (op.empty())throw string("����ʽ�������Ŷ��������ţ���������ʽ");
					}
					catch (string& err)
					{
						cout << endl << "Error: ***" << err << "***" << endl << endl;
						exit(1);
					}
					op.pop(); // ����������
				}
				else {
					// ����ע��, �����������, ������ֻ�ܺ�������ƥ��, ��ˣ�������ǰ���������� op.top() != '('
					while (!op.empty() && op.top() != '(' && lookup_table[ch] <= lookup_table[op.top()]) postfix += op.top(), op.pop();
					op.push(ch);
				}
			}
		}
		while (!op.empty()) {
			char top = op.top(); op.pop();
			try {
				if (top == '(')throw string("����ʽ�������Ŷ��������ţ���������ʽ");
			}
			catch (string& err) {
				cout << endl << "Error: ***" << err << "***" << endl << endl;
				exit(1);
			}
			postfix += top;
		}
		return postfix;
	}

	// Ϊ����ʽ������������
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
		// Ԥ����: ������ӷ�, ת��Ϊ��׺��ʽ
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
						throw (string("�� regexp.h �ļ������ regexp2NFA �˳�����⵽δ֪�ķ���:") + ch);
					}
				}
				else throw (string("����ʽ�м�⵽�Ƿ��ַ�: ") + ch);
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

	// �����ʵ��� NFA ��������кͼ���, һ��ת�Ƶĸ�������, ֻ���űߵĽ�������б���
	void vis_NFA_nodes(N_node *cur_node, char ch, queue<N_node*> &bfs, set<N_node*> &vis) {
		vector<N_node*> *next_nodes = cur_node->next[ch];
		for (auto n : *next_nodes) {
			if (vis.count(n))continue;
			vis.insert(n);
			bfs.push(n);
		}
	}

	// �������е�NFA���ѹ�����, ��������
	void push_NFA_node_2_queue(queue<N_node*> &q, set<N_node*> &s) {
		for (auto n : s)q.push(n);
	}

	// һ��ת��, �ű߿����߶ಽ, ���������ַ���ֻ����һ��, ������dfs����bfs
	set<N_node*> *move(set<N_node*> *cur, char ch) {
		queue<N_node*> bfs;
		set<N_node*> vis;// vis����2��: 1. bfs ����  2. ��¼move�����еĽ��, ����vis�õ��ľ��Ǳ��εĴ�̬��㼯
		set<N_node*> *node_set = nullptr;
		bool init = false;
		// һ��ת��
		for (auto node : *cur) {
			if (node->has_char(ch)) {
				if (init == false)init = true;
				vis_NFA_nodes(node, ch, bfs, vis);
			}
		}
		// ��űհ�
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

	// ���� ��ʼ DFA ���
	D_node *generate_start_node(int node_id, NFA *nfa) {
		const int status = (1 << DFA_space::START);
		D_node *new_dnode = nullptr;
		set<N_node*> *node_set = new set<N_node *>;
		node_set->insert(nfa->get_start());
		// ͨ�������ű�����ʼ��㼯
		set<N_node*> *n_ptrs = move(node_set, epsilon);
		// ����㼯��Ϊ nullptr, ��˵������������ʼ���, ��֮, ��ʼ����Ӧ����Щ�ڵ�ϲ�
		if (n_ptrs != nullptr) {
			for (auto node : *n_ptrs) node_set->insert(node);
			n_ptrs->clear();
			delete n_ptrs;
		}
		// ������ʼ���
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

		// ���������Ҫͨ��һ��ת��������ÿ��DFA����NFA��㼯�ϣ���ô������Ҫͨ��һ������㼯��������DFA
		DFA *new_dfa = new DFA;
		// ��ȡ��ĸ��
		set<char> *alpha_set = get_alpha_table(nfa->get_regexp());
		new_dfa->set_alpha_table(alpha_set);
		// ���������� �ű� �õ���ʼ���
		D_node *start = generate_start_node(new_dfa->get_next_id(), nfa);
		new_dfa->update_next_id(); // ���� next id
		// ���ڱհ�����������Ԥ����, ��Ϊ��ʾ������ͬʱҲ���ս�ڵ�
		start->status |= get_DFA_node_state(start->NFA_nodes);
		new_dfa->set_start_node(start);

		// ��ʼһ��ת��
		queue<D_node*> bfs;
		bfs.push(start);
		new_dfa->add_nfa_node_set(start->NFA_nodes);// ������
		while(!bfs.empty()) {
			D_node *cur = bfs.front(); bfs.pop();
			// �������ܵ��ַ�
			for (auto ch : *alpha_set) {
				set<N_node*> *next = move(cur->NFA_nodes, ch);
				// û�е�ǰ�ַ�
				if (next == nullptr)continue;
				// ���ظ�, ˵���γɱհ�, ��������ͬʱ�ͷ��ڴ�, �ͷ��ڴ�����Ϊnext�д洢�������Ѿ��洢����
				// ����������ͷ�, ���ᵼ���ڴ�й©
				if (new_dfa->has_nfa_node_set(next)) {
					D_node * dptr = new_dfa->get_D_node_addr_by_set(next);
					cur->add_next(ch, dptr);
					delete next;
				}
				// �µĽ��, �������, ����ӳ���ϵ
				else {
					new_dfa->add_nfa_node_set(next); // ����
					D_node *new_dn = new D_node(new_dfa->get_next_id(), get_DFA_node_state(next), next);
					new_dfa->update_next_id();
					cur->add_next(ch, new_dn);
					bfs.push(new_dn);
					new_dfa->add_map_set2NFA_node_addr(next, new_dn);// ����ӳ���ϵ
				}
			}
		}
		new_dfa->set_regexp(nfa->get_regexp());
		return new_dfa;
	}
}
