#pragma once
#include "stdafx.h"
#include "operator.h" // 添加符号支持
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