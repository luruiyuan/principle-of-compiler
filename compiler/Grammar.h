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

class Grammer
{
public:
	Grammer();
	~Grammer();

	// ��ÿ������ʽ���зָ�
	void split_production(string &pro, string &left_part, set<string> &right_part) {
		size_t index = pro.find(this->arrow);
		if (index == string::npos) {
			cout << "����ʽû��->���ţ��������ʽ" << endl;
			return;
		}
		// ����ʽ��
		left_part = pro.substr(0, index);
		// ����ʽ�Ҳ�
		index += arrow.length();
		string right_str = pro.substr(index, pro.length() - index);
		while ((index = right_str.find(or)) != string::npos) {
			right_part.insert(right_str.substr(0, index));
			index += or .length();
			right_str = right_str.substr(index, right_str.length() - index);
		}
		right_part.insert(right_str);
	}

	// ���ݲ���ʽ�󲿺��Ҳ���ȥ��ݹ�, �������res_map��
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
		res.insert(epsilon_str);// epsilon����alpha���е�, ���Һ�����;
		if (!res_map.count(new_left))res_map[new_left] = res;
	}

private:
	static string arrow, or ;
	map<string, set<string>> production_set; // �ķ��Ĳ���ʽ����, ����һ�����ս�����ܶ�Ӧ�������ʽ, �����set�洢
	string start_symbol; // �ķ��Ŀ�ʼ����
	set<string> terminal, non_terminal; // �ķ����ս������, �ķ��ķ��ս������
};

string Grammer::arrow = "->";
string Grammer:: or = "|";

Grammer::Grammer()
{
}

Grammer::~Grammer()
{
}