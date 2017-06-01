#pragma once
#include "stdafx.h"
#include "operator.h" // ��ӷ���֧��
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
#include <fstream>

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
					connect_nfa= (*front_nfa) + (*back_nfa);
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



DFA* nfa2dfa(NFA *nfa) {
	if (nfa->get_start() == nullptr)return nullptr;

	DFA *new_dfa = new DFA;
	// get alphabet table
	set<char> *alpha_set=new set<char>;
	for (auto ch : nfa->get_regexp())
		if (is_alpha(ch) || is_digit(ch))
			alpha_set->insert(ch);
	new_dfa->set_alpha_table(alpha_set);

	N_node *nn = nfa->get_start();
	queue<N_node *> bfs;
	set<N_node*> vis;
	bfs.push(nn);
	vis.insert(nn);


	return new_dfa;
}