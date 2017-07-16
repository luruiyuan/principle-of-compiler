#pragma once

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

// 每一个单元格都是一个 Cell
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

	// 每一定数量字母后换行
	int insert_line(string& s, int line_width=Cell::line_width) {
		if ((int)s.length() <= line_width)return this->insert_line(this->get_height(), s);
		char *buff = new char[line_width + 1];
		int index;
		buff[line_width] = '\0';
		for (int i = 0; i < (int)s.length(); i++) {
			if (i > 0 && i%line_width == 0) index = insert_line(this->get_height(), string(buff));
			buff[i%line_width] = s[i];
		}
		int rest_len = (int)s.length() % line_width;
		if (rest_len) {
			buff[rest_len] = '\0';
			index = insert_line(get_height(), string(buff));
		}
		delete[] buff;
		return index;
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

	// 将一个长字符串插入后得到多少行
	int get_line_num_of_contents(string &content, const int line_width=Cell::line_width) {
		int len = (int)content.length();
		int rest = (int)content.length() % line_width;
		return (int)content.length() / line_width + ((int)content.length() % line_width > 0);
	}

	// 添加空行
	int add_new_blan_lines(int num=1) {
		while ((num--) > 0)this->insert_line(string());
		return this->get_height();
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
	friend ostream& operator<< (ostream& out, Cell& Cell) {
		for (auto line : Cell.get_all_lines())
			out << line;
		return out;
	}

private:
	vector<string> lines;
	int width, height, sep_width;
	const static int line_width;
};

const int Cell::line_width = 40;

Cell::Cell()
{
	this->width = this->height = 0;
}

Cell::~Cell()
{
	this->lines.clear();
}
