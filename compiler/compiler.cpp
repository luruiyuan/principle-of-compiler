// compiler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
using namespace std;

// ��������
#include "table.h"
// ��������ʽ������
#include "regexp.h"
using namespace reg2NFA;
using namespace NFA2DFA;

// ����NFA
#include "NFA.h"



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

int main()
{
	// ����ڴ�й¶
	EnableMemLeakCheck();
	//_CrtSetBreakAlloc(1037);

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
	string n1 = "ab", n2 = "a|b", n3 = "a*", n4 = "ab*", n5 = "a*b", n6 = "a*b*", n7 = "(a|b)*(aa|bb)(a|b)*";
	////string n4 = "(ab|a)(a|bc*)*";
	NFA *nfa1 = regexp2NFA(n1);
	NFA* nfa2 = regexp2NFA(n2);
	NFA* nfa3 = regexp2NFA(n3);
	NFA* nfa4 = regexp2NFA(n4);
	NFA* nfa5 = regexp2NFA(n5);
	NFA* nfa6 = regexp2NFA(n6);
	NFA* nfa7 = regexp2NFA(n7);
	//NFA* nfa4 = regexp2NFA(n4);
	cout << "���nfa�Ա���" << endl;
	//cout << *nfa1 << *nfa2 << *nfa3 << *nfa4;

	cout << *nfa1 << *nfa2 << *nfa3 << endl;
	cout << *nfa4 << *nfa5 << *nfa6 << endl;
	cout << *nfa7 << endl;
	cout << "����NFAתDFA" << endl;
	DFA *dfa1 = nfa2dfa(nfa1);
	DFA *dfa2 = nfa2dfa(nfa2);
	DFA *dfa3 = nfa2dfa(nfa3);
	DFA *dfa4 = nfa2dfa(nfa4);
	DFA *dfa5 = nfa2dfa(nfa5);
	DFA *dfa6 = nfa2dfa(nfa6);
	DFA *dfa7 = nfa2dfa(nfa7);
	cout << *dfa1;
	cout << *dfa2;
	cout << *dfa3;
	cout << *dfa4;
	cout << *dfa5;
	cout << *dfa6;
	cout << *dfa7;
	cout << "����ʽת��ΪNFA�ɹ�" << endl;
	cout << "��������NFA" << endl;

	////cout << "���NFA: " << *nfa1 << endl;
	////DFA *dfa4 = nfa2dfa(nfa1);
	NFA::destroy_all_nfa();
	cout << "�������" << endl;
	cout << "��������DFA" << endl;
	DFA::destroy_all_dfa();
	cout << "�������" << endl;
    return 0;
}