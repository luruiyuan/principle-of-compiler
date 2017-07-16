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

class Grammer
{
public:
	Grammer();
	~Grammer();

	// 将每个产生式进行分割
	void split_production(string &pro, string &left_part, set<string> &right_part) {
		size_t index = pro.find(this->arrow);
		if (index == string::npos) {
			cout << "产生式没有->符号，请检查产生式" << endl;
			return;
		}
		// 产生式左部
		left_part = pro.substr(0, index);
		// 产生式右部
		index += arrow.length();
		string right_str = pro.substr(index, pro.length() - index);
		while ((index = right_str.find(or)) != string::npos) {
			right_part.insert(right_str.substr(0, index));
			index += or .length();
			right_str = right_str.substr(index, right_str.length() - index);
		}
		right_part.insert(right_str);
	}

	// 根据产生式左部和右部消去左递归, 结果填入res_map中
	void eliminate_left_recursion(string &left, set<string> &right, map<string, set<string>> &res_map) {
		size_t index = string::npos;
		string new_left = left + "'";
		set<string> res, beta, alph;
		for (auto r : right) {
			if ((index = r.find(left)) == 0) {
				index += left.length();
				alph.insert(r.substr(index, r.length() - index));
			}
			else beta.insert(r);
		}
		right.clear();
		for (auto b : beta)res_map[left].insert(b + new_left);
		for (auto a : alph)res.insert(a + new_left);
		res.insert(epsilon_str);// epsilon是与alpha并列的, 并且很特殊;
		if (!res_map.count(new_left))res_map[new_left] = res;
	}

private:
	static string arrow, or ;
	map<string, set<string>> production_set; // 文法的产生式集合, 由于一个非终结符可能对应多个产生式, 因此用set存储
	string start_symbol; // 文法的开始符号
	set<string> terminal, non_terminal; // 文法的终结符集合, 文法的非终结符集合
};

string Grammer::arrow = "->";
string Grammer:: or = "|";

Grammer::Grammer()
{
}

Grammer::~Grammer()
{
}