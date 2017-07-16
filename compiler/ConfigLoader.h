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

	// 设置终结符集
	void set_terminals(set<char> *terminals) {
		this->fileReader->set_terminals(terminals);
	}

	// 设置分隔符集合
	void set_seps(set<char> *seps) {
		this->fileReader->set_seps(seps);
	}

	// 读取配置
	void load_config() {
		if (!this->fileReader->is_opened()) {
			cout << "load_config 中文件打开失败" << endl;
			exit(-4);
		}
		cout << "开始加载配置文件，请稍后..." << endl;
		char *next = nullptr;
		while ((next = NEXT(fileReader)) != nullptr) if (EQU(next, "{")) read_block();
		// 将所有配置集合加入vector
		configMaps.push_back(&metaConfig);
		configMaps.push_back(&rulesConfig);
		configMaps.push_back(&keyControlConfig);
		configMaps.push_back(&keyTypeConfig);
		configMaps.push_back(&arithConfig);
		configMaps.push_back(&relationConfig);
		configMaps.push_back(&booleanConfig);
		configMaps.push_back(&arrayConfig);
		cout << "配置文件加载完成" << endl;
	}

	// 获得所有配置信息
	vector<map<string, string>*> *get_all_type_config_maps() {
		return &configMaps;
	}

	// 获取所有配置键到值的集合
	map<string, string> *get_all_config_key2value() {
		return &configuration_key2value;
	}

	// 获取所有配置值到键的集合
	map<string, string> *get_all_config_vaue2key() {
		return &configuration_value2key;
	}

	// 获取种别码
	int get_type_num(string &s) {
		// 如果可以从vaue2key中查到, 说明不需要使用状态机
		if (conf_key2type_num.count(s))return conf_key2type_num[s];
		
	}

	string & get_type_by_type_num(int type_num) {
		if (type_num < 0 && type_num >= conf_keys.size()) {
			cout << "未知种别码: " << type_num << endl;
			cout << "种别码有效范围为:0-" << conf_keys.size() << endl << "程序退出..." << endl;
			exit(-10);
		}
		return conf_keys[type_num];
	}

private:

	// 读取配置模块
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
				cout << "配置文件中含有未知配置: " << buff << endl;
				cout << "程序退出..." << endl;
				exit(-5);
			}
		}
		else {
			cout << "配置文件找不到 elements 对象, 请检查配置块中的elements项" << endl;
			cout << "程序退出..." << endl;
			exit(-6);
		}
		cur_level--;
	}

	// 判断是否满足elements定义
	bool isElements() {
		FileReader * &fr = fileReader;
		return EQU(NEXT(fr), "elements") && EQU(NEXT(fr), ":") && EQU(NEXT(fr), "{");
	}

	// 读取meta 模块的配置
	void read_meta() {
		load_elements(&metaConfig);
	}

	// 读取 rules
	void read_rules() {
		cout << "进入rulesconfig" << endl;
		load_elements(&rulesConfig);
	}

	// 读取关键字配置
	void read_keywords() {
		read_control();
		read_type();
	}

	// 读取运算符配置
	void read_operator() {
		read_arithmetic();
		read_relation();
		read_boolean();
		read_array();
	}

	// 读取控制流关键字
	void read_control() {
		load_elements(&keyControlConfig);
	}

	// 读取数据类型关键字
	void read_type() {
		load_elements(&keyTypeConfig);
	}
	
	// 读取算数运算符
	void read_arithmetic() {
		load_elements(&arithConfig);
	}

	// 加载关系运算符
	void read_relation() {
		load_elements(&relationConfig);
	}

	// 读取bool运算符
	void read_boolean() {
		load_elements(&booleanConfig);
	}

	// 读取数组运算符
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
				if (EQU(right_value(), "}"))shouldBreak = true;	// }代表该模块配置读取完成  计算右值, 注意计算结果在buff中
				if (EQU(buff, "NULL")) {
					init_buff();
					continue;
				}
				(*m)[left] = buff;								// 建立key到value的关系
				(*m)[buff] = left;								// 建立value到key的关系
				conf_key2type_num[left] = conf_keys.size();		// 配置种别码
				conf_keys.push_back(left);
				configuration_key2value[left] = buff;			// 加入到总体配置
				configuration_value2key[buff] = left;
				init_buff();
				if (shouldBreak)break;
			}
			else CP(left, buff_size, next);
		}
		delete[] left;
	}

	// 计算右值, 并填入buff中
	char* right_value() {
		char * next;
		init_buff();																	// 初始化缓存
		while (!EQU((next = NEXT(fileReader)), ",") && !EQU(next, "}")) assigner(next);
		return next;
	}

	// 将替换后%%中的值放在 buff 中
	void value_replace() {
		char *next;
		FileReader *& fr = fileReader;
		while (!EQU((next = NEXT(fr)), "%")) CP(buff + cur_len, buff_size - cur_len, next);
		if (metaConfig.count(buff + cur_len) == 0) {
			cout << "配置文件引用metaConfig时出错，请检查配置文件中对metaConfig的引用" << endl;
			exit(-3);
		}
		sprintf_s(buff + cur_len, buff_size - cur_len, "%s", metaConfig[buff + cur_len].c_str());
	}

	// 产生正规式
	void generate_regexp() {
		int level = 0;
		char *next;
		// 检查
		while (next = NEXT(fileReader),level != 1 || !EQU(next, ")")){
			if (EQU(next, "(")) level++;
			else if (EQU(next, ")"))level--;
			else if (EQU(next, "$")) {
				cout << "配置文件的正规式不允许嵌套, 程序已退出" << endl;
				exit(1);
			}
			assigner(next);
		}
		buff[cur_len++] = ')';
		buff[cur_len] = '\0';
	}

	// 将[ ]中的内容生成并列字符串用于产生正规式
	void generate_split_chars() {
		char *next, cur = 0, pre = 0;
		int start = cur_len;
		while (!EQU(next = NEXT(fileReader), "]")) {
			for (int i = 0; next[i]; i++) {
				if (pre) {
					for (cur = next[i]; pre <= cur; pre++)  buff[cur_len++] = '|', buff[cur_len++] = pre;
					pre = 0;			// 清空pre
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

	// 根据配置首字母进行不同的运算
	char * assigner(char *next) {
		int start_index = 0;
		switch (next[0]){
		case '}':
		case ',':														// 退出的情况
			break;
		case '%':
			value_replace();											// 替换配置文件中 %% 间的值
			break;
		case '$':
			buff[cur_len++] = '$';										// 添加正规式标识符
			start_index = cur_len;
			generate_regexp();											// 将转换后的字符串填入buff
			trans_reg_2_dfa(start_index);
			break;
		case '\\':
			CP(buff + cur_len, buff_size - cur_len, NEXT(fileReader));	// 转译字符处理
			break;
		case '[':
			generate_split_chars();										// 生成分隔符集 [0-9]
			break;
		default:
			CP(buff + cur_len, buff_size - cur_len, next);				// 普通情况, 直接拷贝
			break;
		}
		cur_len = strlen(buff);
		return next;
	}

	// 将正规式转化为DFA并添加到map中
	void trans_reg_2_dfa(int start) {
		DFA *dfa = nfa2dfa(regexp2NFA(string(buff+start)));				// 将正规式转化为DFA
		simplify_DFA(dfa);											// 化简DFA
		map_reg2dfa[buff] = dfa;
		NFA::destroy_all_nfa();										// 释放内存
		cout << *dfa << endl;
	}

	// 初始化buff
	void init_buff() {
		this->cur_len = 0;
		buff[0] = '\0';
	}

	friend ostream & operator << (ostream &out, ConfigLoader &loader) {
		Table table;
		string t_name = "配置加载器配置加载表";
		table.set_table_name(t_name);															// 设置表名
		vector<Cell> head;
		string s1 = "配置模块名称", s2 = "key", s3 = "映射为", s4 = "value";
		Cell h1, h2, h3, h4;
		h1.insert_line(s1); h2.insert_line(s2), h3.insert_line(s3), h4.insert_line(s4);
		head.push_back(h1), head.push_back(h2), head.push_back(h3), head.push_back(h4);					// 配置表头
		table.set_table_head(head);
		for (int i = 0; i < loader.configMaps.size(); i++) {											// 填写表中内容
			auto &conf = *loader.configMaps[i];
			char* conf_name = loader.configNames[i];
			vector<Cell> line;
			Cell c1, c2, c3, c4;
			c1.insert_line(string(conf_name));
			// 为每行cell添加一个配置项
			int row_num = 0, next = 0, next2 = 0, next4 = 0, pre2 = 0, pre4 = 0;
			for (auto p : conf) {
				// 需要多占用的行数
				next2 = c2.get_line_num_of_contents(string(p.first)) - 1;
				next4 = c4.get_line_num_of_contents(string(p.second)) - 1;
				next = max(next2, next4);
				c3.add_new_blan_lines(row_num); c2.add_new_blan_lines(row_num - pre2); c4.add_new_blan_lines(row_num - pre4);
				c2.insert_line(string(p.first));c3.insert_line(string("--->")), c4.insert_line(string(p.second));
				row_num = next; pre2 = next2; pre4 = next4;				// 更新状态
			}
			line.push_back(c1); line.push_back(c2), line.push_back(c3), line.push_back(c4);
			table.insert_row(line);
		}
		out << table;
		return out;
	}

	char *buff;
	int cur_level, cur_len, buff_size; // type_num 作为种别码, 也作为配置项目的统计数目

	vector<string> conf_keys;				// 用种别码做下标存储conf_key
	map<string, int> conf_key2type_num;		// confkey 到种别码

	map<string, DFA*> map_reg2dfa;				// 通过正规式找到对应的DFA

	FileReader *fileReader;
	
	char configNames[8][20] = {
		"metaConfig","rulesConfig","keyControlConfig","keyTypeConfig",
		"arithConfig","relationConfig", "booleanConfig","arrayConfig"
	};
	vector<map<string, string>*> configMaps; // 所有配置的集合的指针

	map<string, string> configuration_key2value;	// 所有配置的键到值
	map<string, string> configuration_value2key;	// 所有配置的值到键

	// 分开配置, 用于检查配置信息和输出配置信息
	map<string, string> metaConfig;			// 配置文件中的 meta 配置
	map<string, string> rulesConfig;		// 配置文件中的 rules 配置
	map<string, string> arithConfig;		// arithmetic 的配置
	map<string, string> relationConfig;		// relation 的配置
	map<string, string> keyControlConfig;	// 控制关键字的配置
	map<string, string> keyTypeConfig;		// 数据类型的配置
	map<string, string> booleanConfig;		// bool运算符的配置
	map<string, string> arrayConfig;		// 数组运算符的配置
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
	// 释放内存，NFA在生成DFA时已经释放
	DFA::destroy_all_dfa();
}