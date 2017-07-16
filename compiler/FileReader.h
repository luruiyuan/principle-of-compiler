#pragma once
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <set>

using namespace std;

set<char> default_terminals{ '{', '}','[',']',',', '=', ':', ';', '$', '%', '|',
'&','!', '\\', '+', '-', '*', '/','>','<', '(', ')' };
set<char> default_seps{ ' ' };

class FileReader
{
public:

	/*
	char: mode open file mode. r/w as read/write
	single_comm: 单行注释符
	multi_left: 多行注释符左部
	multi_right: 多行注释符右部
	*/
	FileReader(const char *path, const char *mode, const char *single_comm, const char *multi_left, const char *multi_right, set<char> *terminals, set<char> *seps);

	// 关闭文件
	void close() {
		if (!this->opened) return;
		fclose(this->fp);
		this->opened = false;
	}
	
	// 读取预处理, 返回下一个字符,过滤注释
	char get_next_char() {
		bool has_space = false;	   // 是否存在前几行中的空格压缩为一个的标志
		while (cur >= 0) {
			// 将连续的空白符压缩为1个
			int pre_cur = cur, pre_row = row;
			skip_space();
			// 如果有空白符被跳过, 判断是压缩为一个还是过滤掉全部空格
			if ((row > pre_row || pre_cur != cur) && has_valid_ch) has_space = true;
			if (cur == len) {
				read_new_line();
				continue;
			}
			// 是否是单行注释
			if (cur < len && find_str(single_row, read_buff + cur, len - cur)) {
				read_new_line();
				continue;
			}
			// 是否是多行注释
			if (cur < len && find_str(left_multi_row, read_buff + cur, len - cur)) {
				bool right_comm_fit = false; // 是否匹配到右部
				cur += (int)left_multi_row.length();
				while (cur >= 0) {
					// up_bound 为能够匹配的下标上界, 注意right_multi_row的length为size_t 因此up_bound可能大于len
					int up_bound = len - (int)right_multi_row.length();
					// 在当前行匹配到多行注释的右部, 则将注释部分跳过, 继续寻找有效字符
					for (; cur < len && cur <= up_bound; cur++)
						if (find_str(right_multi_row, read_buff + cur, len - cur)) {
							right_comm_fit = true;
							cur += (int)right_multi_row.length();
							break;
						}
					// 当前行未匹配, 则读取下一行继续查找
					if(right_comm_fit)break;
					else read_new_line();
				}
				// 错误处理, 没有匹配到多行注释的右部
				if (cur < 0) {
					cout << endl <<"行: " << row << "列: " << cur + 1 << endl 
						<<"****Error: '没有匹配到注释结束符号: "	<< right_multi_row << "'****" <<endl;
					exit(-1);
				}
				if (right_comm_fit)continue;// 跳过多行注释
			}
			// 如果仍然满足 cur < len，则cur是空格或效字符
			if (cur < len) return has_space ? ' ' : (has_valid_ch = true, read_buff[cur++]);  // has_valid_ch 用于过滤文本开头的空格
			else read_new_line();
		}
		// 文件读取到头, read_new_line中将cur赋值-1
		return EOF;
	}

	// 读取下一个有效单词或终结符
	char *get_next_terminal() {
		int len = 0;
		if (terminals->count(next))return str_buff[len++] = next, str_buff[len++] = next = '\0', str_buff;
		while (len < buff_size && (next = get_next_char()) != EOF) {
			// 分隔符集合, 终结符集合, 如果是刚开始碰到空格, 就省略, 反之作为分隔符
			if (seps->count(next)) {
				if (len == 0)continue;
				return str_buff[len++] = '\0', str_buff;
			}
			// 如果是终结符, 且终结符前无其他字符, 则填入缓冲区
			if (terminals->count(next)) {
				if (len == 0) str_buff[len++] = next, next = '\0';
				return str_buff[len++] = '\0', str_buff;
			}
			str_buff[len++] = next;
		}
		if (len == buff_size) {
			cout << "输入串过长, 终结符输出缓冲区大小为: " << buff_size << endl;
			exit(-3);
		}
		return (len == 0 && next == EOF )? nullptr : str_buff;
	}

	void set_terminals(set<char> *terminals) {
		this->terminals = terminals;
	}

	void set_seps(set<char> *seps) {
		this->seps = seps;
	}

	// 判断文件是否打开
	bool is_opened() {
		return this->opened;
	}

	// 重置文件指针
	// pos: 从pos位置开始, pos=0, 文件开始; pos=1, 文件当前位置; pos=2, 文件末尾
	// offset: 相对于pos的位置, 正数后移, 负数前移
	void reset_fptr(int pos, long offset) {
		fseek(fp, offset, pos);
		read_new_line();
	}

	// 文件指针归零
	void rewind_fptr() {
		rewind(fp);
		read_new_line();
	}

	~FileReader();

private:

	// 跳过空白
	inline void skip_space() {
		if (cur >= 0)while (cur < len && (read_buff[cur] == ' ' || read_buff[cur] == '\t'))cur++;
		else return;
	}

	// 打开文件指针
	void open_file(const char *path, const char *mode) {
		fopen_s(&fp, path, mode);
		if (fp == NULL) {
			cout << "文件不存在，请检查文件路径：<" << path << ">" << endl, error = true;
			exit(-2);
		}
		this->opened = true;
		this->read_buff = new char[buff_size];
		this->str_buff = new char[buff_size];
		this->row = 0;
	}

	// 读取文件一行
	void read_new_line() {
		char *ch = fgets(read_buff, buff_size, fp);
		// 文件读取完成
		if (ch == nullptr) {
			this->len = this->cur = -1;
			read_buff[0] = '\0';
			return;
		}
		// 文件读取未完成, fgets如果读取完一行, 会带有换行符, 如果未都读取完, 下次会继续读取
		this->len = strlen(read_buff), this->cur = 0, this->row++;
		if (read_buff[len - 1] == '\n')read_buff[len - 1] = '\0', len--;
	}

	// 查找buff中是否有字符串可以匹配str, 如果匹配，返回true
	bool find_str(string &str, char *buff, int buff_size) {
		if (buff_size < (int)str.length())return false;
		char *s = &str[0], *b = &buff[0];
		int len = (int)str.length();
		while (len--) if (*s++ != *b++)return false; // 首先取 s++, 然后取*s
		return true;
	}

	FILE *fp; // 当前文件指针
	string file_path;

	// 注释定义
	string single_row, left_multi_row, right_multi_row; // 单行注释，多行注释的左部，多行注释的右部 eg: //, /* 和 */

	int cur, len, row; // cur: 当前字符下标, len: 当前行总长度, row: 当前行号

	bool has_valid_ch; // 是否找到有效字符, 找到有效字符前, 空格全部忽略
	bool exist, opened, error;
	static const int buff_size = 1024;
	char *read_buff, *str_buff; // 从文件读入时缓存, 处理为终结符时的缓存
	char next; // 输出到 str_buff 的下一个字符, 默认为 '\0'

	// 终结符集
	set<char> *terminals;
	set<char> *seps;
};

FileReader::FileReader(const char *path, const char *mode, const char *single_comm = "//", const char *multi_left = "/*", const char *multi_right = "*/", set<char> *terminals=&default_terminals, set<char> *seps=&default_seps) {
	this->read_buff = nullptr;
	this->file_path = path;
	error = false;
	if ((int)file_path.length() <= 0) cout << "传入的文件路径长度错误"<< endl << "请检查路径：<" << path << ">" << endl, error = true;
	if (error) return;
	this->open_file(path, mode);
	if (error)return;
	else cout << "文件:" << path << "打开成功" << endl;
	this->single_row = single_comm;
	this->left_multi_row = multi_left;
	this->right_multi_row = multi_right;
	has_valid_ch = false;
	next = '\0';
	this->terminals = terminals;
	this->seps = seps;
	read_new_line(); // 读取第一行
}

FileReader::~FileReader()
{
	this->close();
	if (read_buff != nullptr) delete[] read_buff;
	if (str_buff != nullptr) delete[] str_buff;
	
	read_buff = nullptr;
}