// compiler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
//#include <fstream>
using namespace std;

// ��������
#include "table.h"
// ��������ʽ������
#include "regexp.h"
using namespace reg2NFA;
using namespace NFA2DFA;

// ����NFA
#include "NFA.h"
#include "DFA.h"

// ����Grammar
#include "Grammar.h"

// ����FileReader
#include "FileReader.h"

// ����ConfigLoader
#include "ConfigLoader.h"


// �ڴ�й©���
#include <stdlib.h>  
#include <crtdbg.h>  


#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif  

void EnableMemLeakCheck()
{
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpFlag);
}

void output_split_production(string &left, set<string> &right) {
	cout << "��: " << left << endl;
	string r;
	for (string s : right)r += s + "|";
	cout << "�Ҳ�: " << r.substr(0, r.length() - 1) << endl;
}

void print_map(map<string, string> &m, char* map_name="\0") {
	cout << "map����: " << map_name << endl;
	for (auto p : m)
		cout << "\t key: \"" << p.first << "\"---> value: \"" << p.second << "\"" << endl;
	cout << "��ӡ���" << endl;
}

int main()
{
	// ����ڴ�й¶
	EnableMemLeakCheck();
	//_CrtSetBreakAlloc(271);

	//cout << "��������ز���" << endl;
	//streambuf* cout_stream = cout.rdbuf();// �洢cout������
	//ofstream out_file("out.txt"); // �����ļ������
	//streambuf* out_file_buf = out_file.rdbuf();// ��ȡ�ļ�������
	//cout.rdbuf(out_file_buf); // �޸�cout�Ļ�����

	//test_init();
	//out_file.close();// �ر������, ʹ�������õ���ˢ

	//cout.rdbuf(cout_stream); // �ָ�cout�����������̨
	//cout << "˳����ɣ�" << endl;

	//cout << "Table�������" << endl;
	//Table table;
	//cout << "���ɲ�������..." << endl;
	//string s1 = "С��", s2 = "dog", s3 = "this is a shit";
	//vector<Cell> row1, row2;
	//Cell cell_11, cell_12;
	//cell_11.insert_line(s1);
	//cell_12.insert_line(s2);
	//cell_12.insert_line(s3);
	//row1.push_back(cell_11);
	//row1.push_back(cell_12);
	//row2.push_back(cell_12);
	//row2.push_back(cell_11);
	//table.insert_row(row1);
	//table.insert_row(row2);
	//table.set_table_name(string("�����Ǿ��к�ı���"));
	//string col = "��һ�б�ͷ", col2 = "�ڶ��б�ͷ";
	//Cell c1, c2;
	//c1.insert_line(col), c2.insert_line(col2);
	//vector<Cell> table_head;
	//table_head.push_back(c1);
	//table_head.push_back(c2);
	//table.set_table_head(table_head);
	//cout << "������" << endl << table << endl;;
	//cout << "abcdefgh" << endl;
	///* Ԥ�ڽ������0

	//     �����Ǿ��к�ı���
	//+--------------+--------------+
	//|��һ�б�ͷ    |�ڶ��б�ͷ    |
	//+--------------+--------------+
	//|С��          |dog           |
	//|              |this is a shit|
	//+--------------+--------------+
	//|dog           |С��          |
	//|this is a shit|              |
	//+--------------+--------------+
	//*/
	//cout << "���Գɹ�!" << endl;

	//cout << "���Ժ�׺���ʽ����" << endl;
	//cout << infix2postfix(string("a+b*3*4")) << endl;

	//cout << "����string��insert" << endl;
	//string s = string("abc");
	//for (int i = 0; i < s.length(); i++) {
	//	cout << "i: " << i << " s[i]: " << s[i] << endl;
	//	char c = i % 10 + '0';
	//	string t = "" + c;
	//	s.insert(i, t);
	//}
	
	//cout << "s: " << s << endl;
	//cout << "�������������" << '��' << endl;

	//cout << "������׺ת��׺" << endl;
	//string s1 = "ab*(a*|(ab)*)", s2 = "(ab|a)(a|bc*)";
	//string s3 = "(ab|c(a(bc)*|ac|ab)*(abc*)*)))))))))))"; // ���Ų�ƥ��Ĳ�������
	//string  t, t2, t3;

	//t = add_link_op(s1);
	//t2 = add_link_op(s2);
	//t3 = add_link_op(s3);

	//cout << "s1: " << s1 << " ������ӷ���: " << add_link_op(s1) << endl;
	//cout << "s2: " << s2 << " ������ӷ���: " << add_link_op(s2) << endl;
	//cout << "s3: " << s3 << " ������ӷ���: " << add_link_op(s3) << endl;
	//cout << infix2postfix(t) << endl;
	//cout << infix2postfix(t2) << endl;
	//cout << infix2postfix(t3) << endl;

	//cout << "���� map 2�ֱ���" << endl;
	//cout << "ʹ���Զ��ƶϵ�auto�ؼ���ʱ, ��ʵ�ǽ�key-valueװ�����pair, �����������ͬ, ��������ָ��, ��pair���൱������" << endl;
	//map<int, char> m;
	//m[0] = '0';
	//m[1] = '1';
	//m[2] = '2';

	//cout << "���� map ����������" << endl;
	//for (map<int, char>::iterator it = m.begin(); it != m.end(); it++) {
	//	cout << "key: " << it->first << " value:" << it->second << endl;
	//}

	//cout << "���� map �� pair ��ʽ�ɶԱ���" << endl;
	//for(auto a_pair:m)
	//	cout << "key: " << a_pair.first << " value:" << a_pair.second << endl;

	//cout << "���Խ��ַ�����ת�����ַ����ķ���" << endl << string({ 'n', '\0' }) << endl;
	//cout << "�ַ������ֲ���ֱ��ת��Ϊ�ַ���, ������Ծ�ͨ����" << endl << string(1) << endl;
	//cout << "���ǿ���ʹ��sprintf��sscanf�Ϳ�������������Ч�ʽϸ�" << endl;

	//char buff[100];
	//buff[0] = 'a', buff[1] = 'b', buff[2] = '\0';
	//cout << "�ַ�����ת�ַ���: " << endl << string(buff) << endl << "����:" << string(buff).length() << endl;
	
	//cout << "���� NFA ���" << endl;
	//N_node a, b, c;
	//cout << a.id << b.id << c.id << endl;

	//int i = 1, j = 2;
	//int &k = i;
	//int fuck = k;
	//fuck = -1;
	//cout << "fuck: " << fuck << " i: " << i << endl;

	// vector ����ֱ���±긳ֵ, ��������
	//vector <int> test_vec;
	//cout << "hehe" << endl;
	//test_vec[0] = test_vec[1] = test_vec[2] = 1;
	//cout << "num:" << endl;
	//cout << test_vec[0] << endl;
	//cout << test_vec[4] << endl << test_vec.size();

	//cout << "���� struct ��ȫ�־�̬����" << endl;
	//N_node no1, no2, no3;
	//cout << no1.id << no2.id << no3.id << endl;
	//cout << "���� struct ��ȫ�־�̬�����ɹ�" << endl;

	//cout << "���� struct �е� map �����ܷ��Զ���ʼ��" << endl;
	//N_node nm1, nm2, nm3;
	//char c;
	//cout << nm1.next.count(c) << endl;
	//cout << "���Գɹ�, �����Զ���ʼ��" << endl;

	//cout << "����sprintf_s���ַ���д��һ����ʽ���ķ��ַ�������" << endl;
	//char buff[101];
	//int a = 14, b = 12, c = 13;
	//sprintf_s(buff, 100, "%d.%d.%d", a, b, c);
	//cout << "���char*:" << buff << "���string: " << string(buff) << endl;
	//cout << "���Գɹ�" << endl;

	//set<int> a, b;
	//set<set<int>> c;
	//a.insert(0); a.insert(1);
	//b.insert(0); b.insert(1);

	//c.insert(a); c.insert(b);
	//cout << "c�о����ж��ٸ�Ԫ�أ�" << endl;
	//cout << c.size() << endl;
	//cout << "����a��b�Ĳ���" << endl;
	//cout << "����a��b�ĵ����������ַһ�£���˵�������������й�" << endl;
	//cout << (c.find(a) == c.find(b)) << endl;
	//cout << "���Գɹ�, ���ݵ�ȷ������set�������ֹأ����ַ�޹�" << endl;
	
	//cout << "����set����Ĵ�����ֵ��������" << endl;
	//set<int> a, b;
	//a.insert(0);
	//b = a;
	//a.clear();
	//set<set<int>> c;
	//c.insert(b);

	//cout << "b�Ĵ�С�����a��bָ��ͬһ������, ��b�Ĵ�СΪ0, ����Ϊ1" << endl;
	//cout << "a�Ĵ�С: " << a.size() << endl << "b�Ĵ�С: " << b.size() << endl;
	//cout << "����set" << endl;
	//b.clear();
	//for (auto p : c) {
	//	cout << "c��b��Ԫ�ظ���: " << p.size() << endl;
	//}
	//cout << "b��Ԫ�ظ���: " << b.size() << endl;

	//set<set<int>> set1;
	//set<int> *set2, *set3;
	//set2 = new set<int>;
	//set3 = new set<int>;
	//set2->insert(2333), set3->insert(1);
	//set1.insert(*set2);
	//set1.insert(*set3);
	//delete set2;
	//delete set3;

	//cout << set1.size() << endl;
	//for (auto s : set1)
	//	for (auto i : s)cout << "�����е�Ԫ��: " << i << endl;


	//set<int> *set11 = new set<int>;
	//set<int> set12;
	//set12.insert(2333);
	//cout << "set11 �ĵ�ַ�仯����:" << set11 << endl;
	//*set11 = set12;
	//cout << "set11 �ĵ�ַ�仯����:" << set11 << endl;
	//cout << "set11 ������: " << *(set11->begin()) << endl;
	//cout << "���Գɹ�! set �ĵ�ַû��" << endl;
	//delete set11;

	//map<char, int> m, *testmap = nullptr;
	//m['a'] = 1;
	//cout << m.count('a') << "  " << m.count(1) << endl;
	//*testmap = m;
	//cout << "������nullptrΪ��ʼֵ������£����ܸ���,�������д�������" << endl;
	//cout << testmap->count('a') << endl;

	//cout << "���� stirng���ܷ��Զ���intתΪstring" << endl;
	//string test = string("����һ���Ӻŷ�����intתΪstring�Ĳ��ԣ����֣�") + string(5);
	//cout << "C++����֧��string��intֱ��ת��, ��Ҫ�����ַ��������sstream" << endl;

	//cout << "���Է�����ջ�ϵ�string�����ܷ��ڲ���ʼ��������Զ�����+=����" << endl;
	//string s;
	//s += "��ȥ";
	//cout << s << endl;

	//cout << "�����ַ��������" << endl;
	//cout << (string() == "") << endl;

	////cout << "��������ʽתΪ NFA" << endl;

	//cout << "����forѭ����ʹ��vector���캯��, ��forѭ������� map ��Ƕ��vector�õ��Ľ�����" << endl;
	//map<int, vector<char>> test_m;
	//for (int i = 0; i < 10; i++) {
	//	test_m[i] = vector<char>();
	//	test_m[i].push_back('a' + i);
	//}
	//cout << "����forѭ�����ܷ����vector" << endl;
	//for (auto p : test_m)cout << "key: " << p.first << " value:" << p.second[0] << endl;
	//cout << "���ֲ�������У��������" << endl;


//////////////////////////////////////////////////////////////////////////
	////string alpha_n = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)(0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)*";
	//string alpha_n = "a*b*";
	//NFA* alpha_nfa = regexp2NFA(alpha_n);
	//cout << "���nfa�Ա���" << endl;
	//cout << *alpha_nfa << endl;
	//cout << "����NFAתDFA" << endl;
	//DFA *alpha_dfa = nfa2dfa(alpha_nfa);
	//cout << *alpha_dfa;
	//cout << "����ʽת��ΪNFA�ɹ�" << endl;

	//cout << "����DFA����" << endl;
	//simplify_DFA(alpha_dfa); cout << *alpha_dfa << endl;
	// //����DFA��������
	//cout << alpha_dfa->is_acceptable(string("ab")) << endl;


	//cout << "��������NFA" << endl;
	//NFA::destroy_all_nfa();
	//cout << "�������" << endl;
	//cout << "��������DFA" << endl;
	//DFA::destroy_all_dfa();
	//cout << "�������" << endl;

	////���Բ���ʽ���﷨����
	//cout << endl << endl;
	//cout << "���Բ���ʽ���﷨����" << endl;
	//Grammer g1;
	//string pro1 = "S->Axy", pro2 = "A->E+E|E*E|E/E|E-E", pro3 = "E->i*i", pro4 = "S->Sabc|abc|bc|c";
	//string left;
	//set<string> right_part;
	//g1.split_production(pro1, left, right_part);
	//output_split_production(left, right_part);	
	//right_part.clear();

	//g1.split_production(pro2, left, right_part);
	//output_split_production(left, right_part);
	//right_part.clear();

	//g1.split_production(pro3, left, right_part);
	//output_split_production(left, right_part);
	//right_part.clear();

	//g1.split_production(pro4, left, right_part);
	//output_split_production(left, right_part);

	//cout << "����������ݹ�: " << pro4 << endl;
	//map<string, set<string>> res_map;
	//g1.eliminate_left_recursion(left, right_part, res_map);
	//cout << "����������" << endl;
	//for (auto p : res_map) {
	//	string res = p.first + "->";
	//	bool has_epsilon = false;
	//	for (auto r : p.second) {
	//		if (r == epsilon_str) {
	//			has_epsilon = true;
	//			continue;
	//		}
	//		res += r + "|";
	//	}
	//	if (has_epsilon)res += "��";
	//	else res = res.substr(0, res.length() - 1);
	//	cout << res << endl;
	//}
	//cout << "�������" << endl;

	//cout << "���Բ���ʽ���﷨�������" << endl;


	/*cout << "���������ļ���ȡ" << endl;
	FileReader fr = FileReader("./lex_config.txt", "r", "#");
	char ch;*/
	/*while ((ch = fr.get_next_char()) != EOF)cout << ch;
	cout << endl << "���Գɹ�" << endl;*/
	/*cout << "���������ļ����ս����ȡ" << endl;
	char *s;
	fr.rewind_fptr();

	while ((s = fr.get_next_terminal()) != nullptr)
	cout << "����:\"" << s << "\"" << endl;
	cout << "���Գɹ�" << endl;
	fr.close();*/

	ConfigLoader loader("./lex_config.txt", "r", "#");
	loader.load_config();
	//for (auto m : *(loader.get_config_maps()))print_map(*m);
	cout << loader << endl;
	

    return 0;
}