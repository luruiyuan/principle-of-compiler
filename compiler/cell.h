#pragma once

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

// 每一个单元格都是一个 cell
class Cell
{
public:
	Cell();
	~Cell();

	int get_width() {
		return this->width;
	}

	int get_height() {
		return this->height;
	}

	int insert_line(string& s) {
		return this->insert_line(this->get_height(), s);
	}

	int insert_line(int index, string& s) {
		if (index < 0 || index > this->get_height()) {
			cout << "下标错误 index: " << index << endl;
			return -1;
		}
		if (index == this->get_height())
			this->lines.push_back(s);
		else this->lines.insert(lines.begin() + index, s);
		this->height = lines.size();
		this->width = s.length() > this->width ? s.length() : this->width;
		return index;
	}

	vector<string> get_all_lines() {
		return this->lines;
	}

	vector<string> get_line(int begin_index, int length) {
		vector<string> res;
		vector<string>::iterator it = this->lines.begin() + begin_index;
		for (int i = 0; i < length; i++, it++) res.push_back(*it);
		return res;
	}

	string& get_line(int index) {
		if (index < 0 || index >= lines.size()) {
			cout << "Cell的 string& get_line(int index) 方法参数错误! index: " << index << endl;
			return string("");
		}
		return lines[index];
	}

	//仅限调试
	friend ostream& operator<< (ostream& out, Cell& cell) {
		for (auto line : cell.get_all_lines())
			out << line;
		return out;
	}

private:
	vector<string> lines;
	int width, height;
};

Cell::Cell()
{
	this->width = this->height = 0;
}

Cell::~Cell()
{
	this->lines.clear();
}
