#pragma once
#include "stdafx.h"
#include <vector>
#include <fstream>
#include <string>
#include <set>

using namespace std;
// LL1 预测分析表
class LL1Table
{
public:
	LL1Table();
	~LL1Table();

	void add_non_terminal(char c) {
		non_terminals.insert(c);
	}

	void add_character(char c) {
		chars.insert(c);
	}

	int get_non_terminal_index(char terminal) {
		if (!non_terminals.count(terminal))return -1;
		set<char>::iterator t = non_terminals.find(terminal), it = non_terminals.begin();
		int i = 0;
		for (; it != t; it++, i++);
		return i;
	}

private:
	set<char> non_terminals, chars; // 非终结符, LL1预测分析表中的各种符号
	vector<vector<string>> products; // LL1 分析表中的表项: 产生式
};

LL1Table::LL1Table()
{
}

LL1Table::~LL1Table()
{
}