// compiler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
using namespace std;

// 包含表单类
#include "table.h"
// 包含正规式处理类
#include "regexp.h"
using namespace reg2NFA;
using namespace NFA2DFA;

// 包含NFA
#include "NFA.h"



// 内存泄漏检测
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
	// 检测内存泄露
	EnableMemLeakCheck();
	//_CrtSetBreakAlloc(1037);

	//cout << "输出流重载测试" << endl;
	//streambuf* cout_stream = cout.rdbuf();// 存储cout缓冲区
	//ofstream out_file("out.txt"); // 创建文件输出流
	//streambuf* out_file_buf = out_file.rdbuf();// 获取文件缓冲区
	//cout.rdbuf(out_file_buf); // 修改cout的缓冲区

	//test_init();
	//out_file.close();// 关闭输出流, 使缓冲区得到冲刷

	//cout.rdbuf(cout_stream); // 恢复cout的输出到控制台
	//cout << "顺利完成！" << endl;

	//cout << "Table对象测试" << endl;
	//Table table;
	//cout << "生成测试用例..." << endl;
	//string s1 = "小明", s2 = "dog", s3 = "this is a shit";
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
	//table.set_table_name(string("这里是居中后的表名"));
	//string col = "第一列表头", col2 = "第二列表头";
	//Cell c1, c2;
	//c1.insert_line(col), c2.insert_line(col2);
	//vector<Cell> table_head;
	//table_head.push_back(c1);
	//table_head.push_back(c2);
	//table.set_table_head(table_head);
	//cout << "输出结果" << endl << table << endl;;
	//cout << "abcdefgh" << endl;
	///* 预期结果如下0

	//     这里是居中后的表名
	//+--------------+--------------+
	//|第一列表头    |第二列表头    |
	//+--------------+--------------+
	//|小明          |dog           |
	//|              |this is a shit|
	//+--------------+--------------+
	//|dog           |小明          |
	//|this is a shit|              |
	//+--------------+--------------+
	//*/
	//cout << "测试成功!" << endl;

	//cout << "测试后缀表达式生成" << endl;
	//cout << infix2postfix(string("a+b*3*4")) << endl;

	//cout << "测试string的insert" << endl;
	//string s = string("abc");
	//for (int i = 0; i < s.length(); i++) {
	//	cout << "i: " << i << " s[i]: " << s[i] << endl;
	//	char c = i % 10 + '0';
	//	string t = "" + c;
	//	s.insert(i, t);
	//}
	
	//cout << "s: " << s << endl;
	//cout << "测试链接运算符" << '·' << endl;

	//cout << "测试中缀转后缀" << endl;
	//string s1 = "ab*(a*|(ab)*)", s2 = "(ab|a)(a|bc*)";
	//string s3 = "(ab|c(a(bc)*|ac|ab)*(abc*)*)))))))))))"; // 括号不匹配的测试用例
	//string  t, t2, t3;

	//t = add_link_op(s1);
	//t2 = add_link_op(s2);
	//t3 = add_link_op(s3);

	//cout << "s1: " << s1 << " 添加连接符后: " << add_link_op(s1) << endl;
	//cout << "s2: " << s2 << " 添加连接符后: " << add_link_op(s2) << endl;
	//cout << "s3: " << s3 << " 添加连接符后: " << add_link_op(s3) << endl;
	//cout << infix2postfix(t) << endl;
	//cout << infix2postfix(t2) << endl;
	//cout << infix2postfix(t3) << endl;

	//cout << "测试 map 2种遍历" << endl;
	//cout << "使用自动推断的auto关键字时, 其实是将key-value装配成了pair, 这与迭代器不同, 迭代器是指针, 而pair则相当于引用" << endl;
	//map<int, char> m;
	//m[0] = '0';
	//m[1] = '1';
	//m[2] = '2';

	//cout << "测试 map 迭代器遍历" << endl;
	//for (map<int, char>::iterator it = m.begin(); it != m.end(); it++) {
	//	cout << "key: " << it->first << " value:" << it->second << endl;
	//}

	//cout << "测试 map 用 pair 方式成对遍历" << endl;
	//for(auto a_pair:m)
	//	cout << "key: " << a_pair.first << " value:" << a_pair.second << endl;

	//cout << "测试将字符常量转换成字符串的方法" << endl << string({ 'n', '\0' }) << endl;
	//cout << "字符和数字不能直接转换为字符串, 这个测试就通不过" << endl << string(1) << endl;
	//cout << "但是可以使用sprintf和sscanf就可以做到，并且效率较高" << endl;

	//char buff[100];
	//buff[0] = 'a', buff[1] = 'b', buff[2] = '\0';
	//cout << "字符数组转字符串: " << endl << string(buff) << endl << "长度:" << string(buff).length() << endl;
	
	//cout << "测试 NFA 结点" << endl;
	//N_node a, b, c;
	//cout << a.id << b.id << c.id << endl;

	//int i = 1, j = 2;
	//int &k = i;
	//int fuck = k;
	//fuck = -1;
	//cout << "fuck: " << fuck << " i: " << i << endl;

	// vector 不能直接下标赋值, 测试如下
	//vector <int> test_vec;
	//cout << "hehe" << endl;
	//test_vec[0] = test_vec[1] = test_vec[2] = 1;
	//cout << "num:" << endl;
	//cout << test_vec[0] << endl;
	//cout << test_vec[4] << endl << test_vec.size();

	//cout << "测试 struct 的全局静态变量" << endl;
	//N_node no1, no2, no3;
	//cout << no1.id << no2.id << no3.id << endl;
	//cout << "测试 struct 的全局静态变量成功" << endl;

	//cout << "测试 struct 中的 map 对象能否自动初始化" << endl;
	//N_node nm1, nm2, nm3;
	//char c;
	//cout << nm1.next.count(c) << endl;
	//cout << "测试成功, 可以自动初始化" << endl;

	//cout << "测试sprintf_s向字符串写入一个格式化的非字符串数据" << endl;
	//char buff[101];
	//int a = 14, b = 12, c = 13;
	//sprintf_s(buff, 100, "%d.%d.%d", a, b, c);
	//cout << "输出char*:" << buff << "输出string: " << string(buff) << endl;
	//cout << "测试成功" << endl;

	//set<int> a, b;
	//set<set<int>> c;
	//a.insert(0); a.insert(1);
	//b.insert(0); b.insert(1);

	//c.insert(a); c.insert(b);
	//cout << "c中究竟有多少个元素？" << endl;
	//cout << c.size() << endl;
	//cout << "对于a和b的查找" << endl;
	//cout << "查找a和b的迭代器如果地址一致，就说明仅仅和内容有关" << endl;
	//cout << (c.find(a) == c.find(b)) << endl;
	//cout << "测试成功, 内容的确仅仅和set的内容又关，与地址无关" << endl;
	
	//cout << "测试set对象的传递是值还是引用" << endl;
	//set<int> a, b;
	//a.insert(0);
	//b = a;
	//a.clear();
	//set<set<int>> c;
	//c.insert(b);

	//cout << "b的大小：如果a和b指向同一个对象, 则b的大小为0, 否则为1" << endl;
	//cout << "a的大小: " << a.size() << endl << "b的大小: " << b.size() << endl;
	//cout << "遍历set" << endl;
	//b.clear();
	//for (auto p : c) {
	//	cout << "c中b的元素个数: " << p.size() << endl;
	//}
	//cout << "b的元素个数: " << b.size() << endl;

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
	//	for (auto i : s)cout << "集合中的元素: " << i << endl;


	//set<int> *set11 = new set<int>;
	//set<int> set12;
	//set12.insert(2333);
	//cout << "set11 的地址变化了吗:" << set11 << endl;
	//*set11 = set12;
	//cout << "set11 的地址变化了吗:" << set11 << endl;
	//cout << "set11 的内容: " << *(set11->begin()) << endl;
	//cout << "测试成功! set 的地址没变" << endl;
	//delete set11;

	//map<char, int> m, *testmap = nullptr;
	//m['a'] = 1;
	//cout << m.count('a') << "  " << m.count(1) << endl;
	//*testmap = m;
	//cout << "测试在nullptr为初始值的情况下，不能复制,下面这行代码会出错" << endl;
	//cout << testmap->count('a') << endl;

	//cout << "测试 stirng后能否自动将int转为string" << endl;
	//string test = string("这是一个加号方法将int转为string的测试，数字：") + string(5);
	//cout << "C++不能支持string和int直接转换, 需要借助字符数组或者sstream" << endl;

	//cout << "测试分配在栈上的string对象能否在不初始化情况下自动进行+=操作" << endl;
	//string s;
	//s += "我去";
	//cout << s << endl;

	//cout << "测试字符串相等性" << endl;
	//cout << (string() == "") << endl;

	////cout << "测试正规式转为 NFA" << endl;
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
	cout << "输出nfa以便检查" << endl;
	//cout << *nfa1 << *nfa2 << *nfa3 << *nfa4;

	cout << *nfa1 << *nfa2 << *nfa3 << endl;
	cout << *nfa4 << *nfa5 << *nfa6 << endl;
	cout << *nfa7 << endl;
	cout << "测试NFA转DFA" << endl;
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
	cout << "正规式转换为NFA成功" << endl;
	cout << "销毁所有NFA" << endl;

	////cout << "输出NFA: " << *nfa1 << endl;
	////DFA *dfa4 = nfa2dfa(nfa1);
	NFA::destroy_all_nfa();
	cout << "销毁完成" << endl;
	cout << "销毁所有DFA" << endl;
	DFA::destroy_all_dfa();
	cout << "销毁完成" << endl;
    return 0;
}