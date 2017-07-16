#pragma once

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "Cell.h"
using namespace std;

// 表单类
class Table
{
public:
	Table();
	~Table();

	Cell& get_Cell(int row, int col) {
		return this->content[row][col];
	}

	vector<vector<Cell>> get_content() {
		return this->content;
	}

	vector<vector<Cell>> get_rows(int begin_index, int length) {
		vector<vector<Cell>> rows;
		for (vector<vector<Cell>>::iterator it = content.begin() + begin_index; length > 0; length--) {
			rows.push_back(*it);
		}
		return rows;
	}

	bool has_row(int row_index) {
		return row_index >= 0 && row_index <= this->row_num;
	}

	int delete_row(int index) {
		if (has_row(index))return content.erase(content.begin() + index), index;
		return -1;
	}

	int insert_row(int index, vector<Cell>& row) {
		if (!has_row(index))
			return -1;
		if (index == this->get_row_num()) {
			this->content.push_back(row);
		}
		else {
			vector<vector<Cell>>::iterator it = this->content.begin() + index;
			this->content.insert(it, row);
		}
		this->row_num++;
		this->col_num = this->col_num > row.size() ? this->col_num : row.size();
		return index;
	}

	int insert_row(vector<Cell>& row) {
		return insert_row(content.size(), row);
	}

	int get_row_num() {
		return this->row_num;
	}

	int get_col_num() {
		return this->col_num;
	}

	string& get_table_name() {
		return this->name;
	}

	void set_table_name(string& s) {
		this->name = s;
	}

	vector<Cell>& get_table_head() {
		return this->table_head;
	}

	void set_table_head(vector<Cell>& head) {
		// 如果表为空或者表头的列数和表的列数一致, 则视为有效表头, 否则报错
		try
		{
			if (this->col_num == 0 || head.size() == this->col_num)
				this->table_head = head;
			else throw string("表头的列数与表中内容的列数不相等，表头设置失败");
		}
		catch (string& err)
		{
			cout << "Error: " << "***" << err << "***" << endl;
		}

	}

	friend ostream& operator << (ostream& out, Table& table) {
		// 如果表头内容有效，将表头加入输出内容列表, 输出后从内容中删除
		bool add_head = false;
		if (table.table_head.size() == table.col_num)table.insert_row(0, table.table_head), add_head = true;

		int row = table.get_row_num(), col = table.get_col_num(); // 获取表格的行数和列数
		if (row <= 0 || col <= 0){
			out << "表格为空, 打印结束" << endl;
			return out;
		}
		// 获取并存储每列的最大宽度和最大高度
		int *col_widths = new int[col], *row_heights = new int[row];
		memset(col_widths, 0, col * sizeof(int));
		memset(row_heights, 0, row * sizeof(int));
		vector<vector<Cell>>&content = table.get_content();

		for (int i = 0; i < row;i++) {
			for (int j = 0; j < col;j++) {
				int width = content[i][j].get_width(), height = content[i][j].get_height();
				row_heights[i] = row_heights[i] > height ? row_heights[i] : height;
				col_widths[j] = col_widths[j] > width ? col_widths[j] : width;
			}
		}
		// 居中打印输出表名并换行
		int whole_width = 0;
		for (int i = 0; i < col; i++)whole_width += col_widths[i];
		mid_line(table.get_table_name(), whole_width, out);
		out << endl;
		
		//输出第一行, 第一行一定是横线
		sep_line(col_widths, col, out);

		// 绘制一行内容以及每行之后的分割线, 注意每个Cell内可能有多行内容  例如,
		//|小明|dog           |
		//|    |this is a shit|
		//+----+--------------+
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < row_heights[i]; j++) { // j 为每个Cell内的行数
				out << "|";
				for (int k = 0; k < content[i].size();k++) { // 遍历每行k个Cell
					Cell Cell = content[i][k];
					int sp = col_widths[k]; // sp记录打印的空格个数
					// 有有效输出时, 输出内容,并用空格补足空位, 否则直接输出空格占位
					if (j < Cell.get_height()) {
						string s = Cell.get_line(j);
						out << s;
						sp -= s.length();
					}
					while (sp--)out << " ";
					out << "|";
				}
				out << endl;
			}
			sep_line(col_widths, col, out);// 每行输出完成后画底线
		}

		if(add_head)content.erase(content.begin());// 将表头从输出内容中删除
		delete[] col_widths;
		delete[] row_heights;
		return out;
	}

private:
	vector<vector<Cell>> content;
	vector<Cell> table_head; // 表头
	int row_num, col_num;
	string name;

	// 绘制一行分割线 +------+--+回车
	static inline void sep_line(int *col_width, int len, ostream& out) {
		out << "+";
		for (int i = 0; i < len; i++){
			for (int j = 0; j < col_width[i]; j++) {
				out << "-";
			}
			out << "+";
		}
		out << endl;
	}

	// 将某一行从当前光标位置，在长度为 whole_width 的情况下, 居中输出
	// 光标可以不从行首开始, 因此也适用于表格
	static inline void mid_line(string&s, int whole_width, ostream&out) {
		register int len = (whole_width - (int)s.length()) / 2; // s.length() 是 size_t 类型，即 unsigned int 不做强制类型转换就错了
		len = len > 0 ? len : 0;
		while (len--)out << " ";
		out << s;
	}

};

Table::Table()
{
	this->row_num = this->col_num = 0;
	this->name = "";
}

Table::~Table()
{
	this->name.clear();
	for (auto vec : content) {
		for (auto Cell : vec)
			Cell.~Cell();
		vec.clear();
	}
	content.clear();
}
