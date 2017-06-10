#pragma once
#include "stdafx.h"
#include <vector>
#include <fstream>
#include <string>
#include <set>

using namespace std;
// LL1 Ԥ�������
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
	set<char> non_terminals, chars; // ���ս��, LL1Ԥ��������еĸ��ַ���
	vector<vector<string>> products; // LL1 �������еı���: ����ʽ
};

LL1Table::LL1Table()
{
}

LL1Table::~LL1Table()
{
}