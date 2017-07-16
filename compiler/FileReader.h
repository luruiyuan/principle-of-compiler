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
	single_comm: ����ע�ͷ�
	multi_left: ����ע�ͷ���
	multi_right: ����ע�ͷ��Ҳ�
	*/
	FileReader(const char *path, const char *mode, const char *single_comm, const char *multi_left, const char *multi_right, set<char> *terminals, set<char> *seps);

	// �ر��ļ�
	void close() {
		if (!this->opened) return;
		fclose(this->fp);
		this->opened = false;
	}
	
	// ��ȡԤ����, ������һ���ַ�,����ע��
	char get_next_char() {
		bool has_space = false;	   // �Ƿ����ǰ�����еĿո�ѹ��Ϊһ���ı�־
		while (cur >= 0) {
			// �������Ŀհ׷�ѹ��Ϊ1��
			int pre_cur = cur, pre_row = row;
			skip_space();
			// ����пհ׷�������, �ж���ѹ��Ϊһ�����ǹ��˵�ȫ���ո�
			if ((row > pre_row || pre_cur != cur) && has_valid_ch) has_space = true;
			if (cur == len) {
				read_new_line();
				continue;
			}
			// �Ƿ��ǵ���ע��
			if (cur < len && find_str(single_row, read_buff + cur, len - cur)) {
				read_new_line();
				continue;
			}
			// �Ƿ��Ƕ���ע��
			if (cur < len && find_str(left_multi_row, read_buff + cur, len - cur)) {
				bool right_comm_fit = false; // �Ƿ�ƥ�䵽�Ҳ�
				cur += (int)left_multi_row.length();
				while (cur >= 0) {
					// up_bound Ϊ�ܹ�ƥ����±��Ͻ�, ע��right_multi_row��lengthΪsize_t ���up_bound���ܴ���len
					int up_bound = len - (int)right_multi_row.length();
					// �ڵ�ǰ��ƥ�䵽����ע�͵��Ҳ�, ��ע�Ͳ�������, ����Ѱ����Ч�ַ�
					for (; cur < len && cur <= up_bound; cur++)
						if (find_str(right_multi_row, read_buff + cur, len - cur)) {
							right_comm_fit = true;
							cur += (int)right_multi_row.length();
							break;
						}
					// ��ǰ��δƥ��, ���ȡ��һ�м�������
					if(right_comm_fit)break;
					else read_new_line();
				}
				// ������, û��ƥ�䵽����ע�͵��Ҳ�
				if (cur < 0) {
					cout << endl <<"��: " << row << "��: " << cur + 1 << endl 
						<<"****Error: 'û��ƥ�䵽ע�ͽ�������: "	<< right_multi_row << "'****" <<endl;
					exit(-1);
				}
				if (right_comm_fit)continue;// ��������ע��
			}
			// �����Ȼ���� cur < len����cur�ǿո��Ч�ַ�
			if (cur < len) return has_space ? ' ' : (has_valid_ch = true, read_buff[cur++]);  // has_valid_ch ���ڹ����ı���ͷ�Ŀո�
			else read_new_line();
		}
		// �ļ���ȡ��ͷ, read_new_line�н�cur��ֵ-1
		return EOF;
	}

	// ��ȡ��һ����Ч���ʻ��ս��
	char *get_next_terminal() {
		int len = 0;
		if (terminals->count(next))return str_buff[len++] = next, str_buff[len++] = next = '\0', str_buff;
		while (len < buff_size && (next = get_next_char()) != EOF) {
			// �ָ�������, �ս������, ����Ǹտ�ʼ�����ո�, ��ʡ��, ��֮��Ϊ�ָ���
			if (seps->count(next)) {
				if (len == 0)continue;
				return str_buff[len++] = '\0', str_buff;
			}
			// ������ս��, ���ս��ǰ�������ַ�, �����뻺����
			if (terminals->count(next)) {
				if (len == 0) str_buff[len++] = next, next = '\0';
				return str_buff[len++] = '\0', str_buff;
			}
			str_buff[len++] = next;
		}
		if (len == buff_size) {
			cout << "���봮����, �ս�������������СΪ: " << buff_size << endl;
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

	// �ж��ļ��Ƿ��
	bool is_opened() {
		return this->opened;
	}

	// �����ļ�ָ��
	// pos: ��posλ�ÿ�ʼ, pos=0, �ļ���ʼ; pos=1, �ļ���ǰλ��; pos=2, �ļ�ĩβ
	// offset: �����pos��λ��, ��������, ����ǰ��
	void reset_fptr(int pos, long offset) {
		fseek(fp, offset, pos);
		read_new_line();
	}

	// �ļ�ָ�����
	void rewind_fptr() {
		rewind(fp);
		read_new_line();
	}

	~FileReader();

private:

	// �����հ�
	inline void skip_space() {
		if (cur >= 0)while (cur < len && (read_buff[cur] == ' ' || read_buff[cur] == '\t'))cur++;
		else return;
	}

	// ���ļ�ָ��
	void open_file(const char *path, const char *mode) {
		fopen_s(&fp, path, mode);
		if (fp == NULL) {
			cout << "�ļ������ڣ������ļ�·����<" << path << ">" << endl, error = true;
			exit(-2);
		}
		this->opened = true;
		this->read_buff = new char[buff_size];
		this->str_buff = new char[buff_size];
		this->row = 0;
	}

	// ��ȡ�ļ�һ��
	void read_new_line() {
		char *ch = fgets(read_buff, buff_size, fp);
		// �ļ���ȡ���
		if (ch == nullptr) {
			this->len = this->cur = -1;
			read_buff[0] = '\0';
			return;
		}
		// �ļ���ȡδ���, fgets�����ȡ��һ��, ����л��з�, ���δ����ȡ��, �´λ������ȡ
		this->len = strlen(read_buff), this->cur = 0, this->row++;
		if (read_buff[len - 1] == '\n')read_buff[len - 1] = '\0', len--;
	}

	// ����buff���Ƿ����ַ�������ƥ��str, ���ƥ�䣬����true
	bool find_str(string &str, char *buff, int buff_size) {
		if (buff_size < (int)str.length())return false;
		char *s = &str[0], *b = &buff[0];
		int len = (int)str.length();
		while (len--) if (*s++ != *b++)return false; // ����ȡ s++, Ȼ��ȡ*s
		return true;
	}

	FILE *fp; // ��ǰ�ļ�ָ��
	string file_path;

	// ע�Ͷ���
	string single_row, left_multi_row, right_multi_row; // ����ע�ͣ�����ע�͵��󲿣�����ע�͵��Ҳ� eg: //, /* �� */

	int cur, len, row; // cur: ��ǰ�ַ��±�, len: ��ǰ���ܳ���, row: ��ǰ�к�

	bool has_valid_ch; // �Ƿ��ҵ���Ч�ַ�, �ҵ���Ч�ַ�ǰ, �ո�ȫ������
	bool exist, opened, error;
	static const int buff_size = 1024;
	char *read_buff, *str_buff; // ���ļ�����ʱ����, ����Ϊ�ս��ʱ�Ļ���
	char next; // ����� str_buff ����һ���ַ�, Ĭ��Ϊ '\0'

	// �ս����
	set<char> *terminals;
	set<char> *seps;
};

FileReader::FileReader(const char *path, const char *mode, const char *single_comm = "//", const char *multi_left = "/*", const char *multi_right = "*/", set<char> *terminals=&default_terminals, set<char> *seps=&default_seps) {
	this->read_buff = nullptr;
	this->file_path = path;
	error = false;
	if ((int)file_path.length() <= 0) cout << "������ļ�·�����ȴ���"<< endl << "����·����<" << path << ">" << endl, error = true;
	if (error) return;
	this->open_file(path, mode);
	if (error)return;
	else cout << "�ļ�:" << path << "�򿪳ɹ�" << endl;
	this->single_row = single_comm;
	this->left_multi_row = multi_left;
	this->right_multi_row = multi_right;
	has_valid_ch = false;
	next = '\0';
	this->terminals = terminals;
	this->seps = seps;
	read_new_line(); // ��ȡ��һ��
}

FileReader::~FileReader()
{
	this->close();
	if (read_buff != nullptr) delete[] read_buff;
	if (str_buff != nullptr) delete[] str_buff;
	
	read_buff = nullptr;
}