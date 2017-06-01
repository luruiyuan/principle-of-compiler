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
	// ����ڴ�д��
	EnableMemLeakCheck();
	//_CrtSetBreakAlloc(180);

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

	//cout << "��������ʽתΪ NFA" << endl;
	//string n1 = "ab", n2 = "a|b", n3 = "a*";
	//string n4 = "(ab|a)(a|bc*)*";
	//NFA *nfa1 = regexp2NFA(n1);
	//NFA* nfa2 = regexp2NFA(n2);
	//NFA* nfa3 = regexp2NFA(n3);
	//NFA* nfa4 = regexp2NFA(n4);
	//cout << "���nfa�Ա���" << endl;
	//cout << *nfa1 << *nfa2 << *nfa3 << *nfa4;
	//cout << "����ʽת��ΪNFA�ɹ�" << endl;
	//cout << "��������NFA" << endl;
	//NFA::destroy_all_nfa();
	//cout << "�������" << endl;

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

    return 0;
}