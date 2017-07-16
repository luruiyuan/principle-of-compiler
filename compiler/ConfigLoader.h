#pragma once
#include "FileReader.h"
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "table.h"
#include "regexp.h"

using namespace std;

# define EQU(S1,S2) (strcmp(S1, S2) == 0)
# define NEXT(FILEREADER) (FILEREADER->get_next_terminal())
# define CP(DST, SIZE_BYTES, SRC) strcpy_s(DST, SIZE_BYTES, SRC);

class ConfigLoader
{
public:
	ConfigLoader(const char *path, const char *mode, const char *single_comm, const char *multi_left, const char *multi_right);
	~ConfigLoader();

	// �����ս����
	void set_terminals(set<char> *terminals) {
		this->fileReader->set_terminals(terminals);
	}

	// ���÷ָ�������
	void set_seps(set<char> *seps) {
		this->fileReader->set_seps(seps);
	}

	// ��ȡ����
	void load_config() {
		if (!this->fileReader->is_opened()) {
			cout << "load_config ���ļ���ʧ��" << endl;
			exit(-4);
		}
		cout << "��ʼ���������ļ������Ժ�..." << endl;
		char *next = nullptr;
		while ((next = NEXT(fileReader)) != nullptr) if (EQU(next, "{")) read_block();
		// ���������ü��ϼ���vector
		configMaps.push_back(&metaConfig);
		configMaps.push_back(&rulesConfig);
		configMaps.push_back(&keyControlConfig);
		configMaps.push_back(&keyTypeConfig);
		configMaps.push_back(&arithConfig);
		configMaps.push_back(&relationConfig);
		configMaps.push_back(&booleanConfig);
		configMaps.push_back(&arrayConfig);
		cout << "�����ļ��������" << endl;
	}

	// �������������Ϣ
	vector<map<string, string>*> *get_all_type_config_maps() {
		return &configMaps;
	}

	// ��ȡ�������ü���ֵ�ļ���
	map<string, string> *get_all_config_key2value() {
		return &configuration_key2value;
	}

	// ��ȡ��������ֵ�����ļ���
	map<string, string> *get_all_config_vaue2key() {
		return &configuration_value2key;
	}

	// ��ȡ�ֱ���
	int get_type_num(string &s) {
		// ������Դ�vaue2key�в鵽, ˵������Ҫʹ��״̬��
		if (conf_key2type_num.count(s))return conf_key2type_num[s];
		
	}

	string & get_type_by_type_num(int type_num) {
		if (type_num < 0 && type_num >= conf_keys.size()) {
			cout << "δ֪�ֱ���: " << type_num << endl;
			cout << "�ֱ�����Ч��ΧΪ:0-" << conf_keys.size() << endl << "�����˳�..." << endl;
			exit(-10);
		}
		return conf_keys[type_num];
	}

private:

	// ��ȡ����ģ��
	void read_block() {
		char *name;
		for (int i = 0; i < 4; i++) {
			name = NEXT(fileReader);
			if (i == 2) {
				CP(buff, buff_size, name);
				cur_len = strlen(buff);
			}
		}
		if (isElements()) {
			if (EQU(buff, "meta")) cur_level++, read_meta();
			else if (EQU(buff, "rules")) cur_level++, read_rules();
			else if (EQU(buff, "keywords")) cur_level++, read_keywords();
			else if (EQU(buff, "operator")) cur_level++, read_operator();
			else {
				cout << "�����ļ��к���δ֪����: " << buff << endl;
				cout << "�����˳�..." << endl;
				exit(-5);
			}
		}
		else {
			cout << "�����ļ��Ҳ��� elements ����, �������ÿ��е�elements��" << endl;
			cout << "�����˳�..." << endl;
			exit(-6);
		}
		cur_level--;
	}

	// �ж��Ƿ�����elements����
	bool isElements() {
		FileReader * &fr = fileReader;
		return EQU(NEXT(fr), "elements") && EQU(NEXT(fr), ":") && EQU(NEXT(fr), "{");
	}

	// ��ȡmeta ģ�������
	void read_meta() {
		load_elements(&metaConfig);
	}

	// ��ȡ rules
	void read_rules() {
		cout << "����rulesconfig" << endl;
		load_elements(&rulesConfig);
	}

	// ��ȡ�ؼ�������
	void read_keywords() {
		read_control();
		read_type();
	}

	// ��ȡ���������
	void read_operator() {
		read_arithmetic();
		read_relation();
		read_boolean();
		read_array();
	}

	// ��ȡ�������ؼ���
	void read_control() {
		load_elements(&keyControlConfig);
	}

	// ��ȡ�������͹ؼ���
	void read_type() {
		load_elements(&keyTypeConfig);
	}
	
	// ��ȡ���������
	void read_arithmetic() {
		load_elements(&arithConfig);
	}

	// ���ع�ϵ�����
	void read_relation() {
		load_elements(&relationConfig);
	}

	// ��ȡbool�����
	void read_boolean() {
		load_elements(&booleanConfig);
	}

	// ��ȡ���������
	void read_array() {
		load_elements(&arrayConfig);
	}

	void load_elements(map<string, string> *m) {
		FileReader * &fr = fileReader;
		char *next, *left = new char[buff_size];
		while ((next = NEXT(fr)) != nullptr && !EQU(next, "}")) {
			if (EQU(next, ",")) continue;
			else if (EQU(next, "=")) {
				bool shouldBreak = false;
				if (EQU(right_value(), "}"))shouldBreak = true;	// }�����ģ�����ö�ȡ���  ������ֵ, ע���������buff��
				if (EQU(buff, "NULL")) {
					init_buff();
					continue;
				}
				(*m)[left] = buff;								// ����key��value�Ĺ�ϵ
				(*m)[buff] = left;								// ����value��key�Ĺ�ϵ
				conf_key2type_num[left] = conf_keys.size();		// �����ֱ���
				conf_keys.push_back(left);
				configuration_key2value[left] = buff;			// ���뵽��������
				configuration_value2key[buff] = left;
				init_buff();
				if (shouldBreak)break;
			}
			else CP(left, buff_size, next);
		}
		delete[] left;
	}

	// ������ֵ, ������buff��
	char* right_value() {
		char * next;
		init_buff();																	// ��ʼ������
		while (!EQU((next = NEXT(fileReader)), ",") && !EQU(next, "}")) assigner(next);
		return next;
	}

	// ���滻��%%�е�ֵ���� buff ��
	void value_replace() {
		char *next;
		FileReader *& fr = fileReader;
		while (!EQU((next = NEXT(fr)), "%")) CP(buff + cur_len, buff_size - cur_len, next);
		if (metaConfig.count(buff + cur_len) == 0) {
			cout << "�����ļ�����metaConfigʱ�������������ļ��ж�metaConfig������" << endl;
			exit(-3);
		}
		sprintf_s(buff + cur_len, buff_size - cur_len, "%s", metaConfig[buff + cur_len].c_str());
	}

	// ��������ʽ
	void generate_regexp() {
		int level = 0;
		char *next;
		// ���
		while (next = NEXT(fileReader),level != 1 || !EQU(next, ")")){
			if (EQU(next, "(")) level++;
			else if (EQU(next, ")"))level--;
			else if (EQU(next, "$")) {
				cout << "�����ļ�������ʽ������Ƕ��, �������˳�" << endl;
				exit(1);
			}
			assigner(next);
		}
		buff[cur_len++] = ')';
		buff[cur_len] = '\0';
	}

	// ��[ ]�е��������ɲ����ַ������ڲ�������ʽ
	void generate_split_chars() {
		char *next, cur = 0, pre = 0;
		int start = cur_len;
		while (!EQU(next = NEXT(fileReader), "]")) {
			for (int i = 0; next[i]; i++) {
				if (pre) {
					for (cur = next[i]; pre <= cur; pre++)  buff[cur_len++] = '|', buff[cur_len++] = pre;
					pre = 0;			// ���pre
				}
				else if (next[i] == '-') {
					pre = cur + 1;
					if (next[++i]) cur = next[i];
					else break;
				}
				else if (next[i] == '%')buff[cur_len++] = '|', assigner(next+i);
				else buff[cur_len++] = '|', buff[cur_len++] = cur = next[i];
			}
		}
		buff[start] = '(', buff[cur_len++] = ')', buff[cur_len] = '\0';
	}

	// ������������ĸ���в�ͬ������
	char * assigner(char *next) {
		int start_index = 0;
		switch (next[0]){
		case '}':
		case ',':														// �˳������
			break;
		case '%':
			value_replace();											// �滻�����ļ��� %% ���ֵ
			break;
		case '$':
			buff[cur_len++] = '$';										// �������ʽ��ʶ��
			start_index = cur_len;
			generate_regexp();											// ��ת������ַ�������buff
			trans_reg_2_dfa(start_index);
			break;
		case '\\':
			CP(buff + cur_len, buff_size - cur_len, NEXT(fileReader));	// ת���ַ�����
			break;
		case '[':
			generate_split_chars();										// ���ɷָ����� [0-9]
			break;
		default:
			CP(buff + cur_len, buff_size - cur_len, next);				// ��ͨ���, ֱ�ӿ���
			break;
		}
		cur_len = strlen(buff);
		return next;
	}

	// ������ʽת��ΪDFA����ӵ�map��
	void trans_reg_2_dfa(int start) {
		DFA *dfa = nfa2dfa(regexp2NFA(string(buff+start)));				// ������ʽת��ΪDFA
		simplify_DFA(dfa);											// ����DFA
		map_reg2dfa[buff] = dfa;
		NFA::destroy_all_nfa();										// �ͷ��ڴ�
		cout << *dfa << endl;
	}

	// ��ʼ��buff
	void init_buff() {
		this->cur_len = 0;
		buff[0] = '\0';
	}

	friend ostream & operator << (ostream &out, ConfigLoader &loader) {
		Table table;
		string t_name = "���ü��������ü��ر�";
		table.set_table_name(t_name);															// ���ñ���
		vector<Cell> head;
		string s1 = "����ģ������", s2 = "key", s3 = "ӳ��Ϊ", s4 = "value";
		Cell h1, h2, h3, h4;
		h1.insert_line(s1); h2.insert_line(s2), h3.insert_line(s3), h4.insert_line(s4);
		head.push_back(h1), head.push_back(h2), head.push_back(h3), head.push_back(h4);					// ���ñ�ͷ
		table.set_table_head(head);
		for (int i = 0; i < loader.configMaps.size(); i++) {											// ��д��������
			auto &conf = *loader.configMaps[i];
			char* conf_name = loader.configNames[i];
			vector<Cell> line;
			Cell c1, c2, c3, c4;
			c1.insert_line(string(conf_name));
			// Ϊÿ��cell���һ��������
			int row_num = 0, next = 0, next2 = 0, next4 = 0, pre2 = 0, pre4 = 0;
			for (auto p : conf) {
				// ��Ҫ��ռ�õ�����
				next2 = c2.get_line_num_of_contents(string(p.first)) - 1;
				next4 = c4.get_line_num_of_contents(string(p.second)) - 1;
				next = max(next2, next4);
				c3.add_new_blan_lines(row_num); c2.add_new_blan_lines(row_num - pre2); c4.add_new_blan_lines(row_num - pre4);
				c2.insert_line(string(p.first));c3.insert_line(string("--->")), c4.insert_line(string(p.second));
				row_num = next; pre2 = next2; pre4 = next4;				// ����״̬
			}
			line.push_back(c1); line.push_back(c2), line.push_back(c3), line.push_back(c4);
			table.insert_row(line);
		}
		out << table;
		return out;
	}

	char *buff;
	int cur_level, cur_len, buff_size; // type_num ��Ϊ�ֱ���, Ҳ��Ϊ������Ŀ��ͳ����Ŀ

	vector<string> conf_keys;				// ���ֱ������±�洢conf_key
	map<string, int> conf_key2type_num;		// confkey ���ֱ���

	map<string, DFA*> map_reg2dfa;				// ͨ������ʽ�ҵ���Ӧ��DFA

	FileReader *fileReader;
	
	char configNames[8][20] = {
		"metaConfig","rulesConfig","keyControlConfig","keyTypeConfig",
		"arithConfig","relationConfig", "booleanConfig","arrayConfig"
	};
	vector<map<string, string>*> configMaps; // �������õļ��ϵ�ָ��

	map<string, string> configuration_key2value;	// �������õļ���ֵ
	map<string, string> configuration_value2key;	// �������õ�ֵ����

	// �ֿ�����, ���ڼ��������Ϣ�����������Ϣ
	map<string, string> metaConfig;			// �����ļ��е� meta ����
	map<string, string> rulesConfig;		// �����ļ��е� rules ����
	map<string, string> arithConfig;		// arithmetic ������
	map<string, string> relationConfig;		// relation ������
	map<string, string> keyControlConfig;	// ���ƹؼ��ֵ�����
	map<string, string> keyTypeConfig;		// �������͵�����
	map<string, string> booleanConfig;		// bool�����������
	map<string, string> arrayConfig;		// ���������������
};

ConfigLoader::ConfigLoader(const char *path, const char *mode, const char *single_comm = "//", const char *multi_left = "/*", const char *multi_right = "*/")
{
	fileReader = new FileReader(path, mode, single_comm, multi_left, multi_right);
	cur_level = 0;
	buff_size = 1024;
	buff = new char[buff_size];
}

ConfigLoader::~ConfigLoader()
{
	delete fileReader, fileReader = nullptr;
	delete[] buff, buff = nullptr;
	// �ͷ��ڴ棬NFA������DFAʱ�Ѿ��ͷ�
	DFA::destroy_all_dfa();
}