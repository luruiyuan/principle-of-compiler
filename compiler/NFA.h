#pragma once
#include "stdafx.h"
#include <fstream>
#include <map>
#include <stack>
#include <queue>
#include "operator.h"
#include "table.h"

using namespace std;
using namespace NFA_space;

typedef struct NFA_Node N_node;
struct NFA_Node {
	int id, state; // 结点 id 和 结点 状态
	map<char, vector<N_node*>*> next; // NFA 次态
	static int next_id;

	// init
	NFA_Node() {
		this->id = next_id++;
		this->state = NFA_space::MID;
	}

	// 析构函数
	~NFA_Node() {
		for (map<char, vector<N_node*>*>::iterator it = next.begin(); it != next.end(); it++) {
			vector<N_node*> *t = it->second;
			t->clear();
			delete t;
		}
	}

	bool has_char(char ch) {
		return next.count(ch) > 0;
	}

	bool is_end() {
		return state == NFA_space::END;
	}
	bool is_start() {
		return state == NFA_space::START;
	}
	bool is_mid() {
		return state == NFA_space::MID;
	}

	bool has_next() {
		return this->next.size() > 0;
	}

	// 插入结点, 返回节点对应的下标
	int add_next_node(char ch, N_node& node) {
		if (has_char(ch))next[ch]->push_back(&node);
		else {
			vector<N_node*> *t = new vector<N_node*>;
			t->push_back(&node);
			next[ch] = t;
		}
		return next[ch]->size() - 1;
	}

	bool has_next(char ch, int node_id) {
		if (has_char(ch)) {
			for (auto node : (*next[ch])) {
				if (node->id == node_id)return true;
			}
		}
		return false;
	}

	bool has_next(char ch, NFA_Node& node) {
		return has_next(ch, node.id);
	}

	vector<N_node*> get_next_nodes(char ch) {
		return (*next[ch]);
	}
};

int N_node::next_id = 0; // 定义语句本应放在cpp文件中

// 定义NFA类
class NFA
{
public:
	NFA();
	~NFA();

	NFA(char ch);

	// 当一个 NFA 构造完成, 将其起始结点和终止结点初始化为正确的状态
	void init_start_end_node() {
		this->start->state = NFA_space::START;
		this->end->state = NFA_space::END;
	}

	static void destroy_all_nfa() {
		NFA::vis_N_node_destroy.clear();
		while (!NFA::NFA_destroy.empty()) {
			NFA *ptr = NFA_destroy.top(); NFA_destroy.pop();
			delete ptr;
		}
	}

	void set_regexp(string& s) {
		this->regexp = s;
	}

	string get_regexp() {
		return this->regexp;
	}

	N_node *get_start() {
		return this->start;
	}

private:
	N_node *start, *end;
	string regexp;

	// 销毁 NFA 的声明, 写在类外才是定义, 如果不写会报 link 2001 错误
	static stack<NFA*> NFA_destroy;
	static set<N_node*> vis_N_node_destroy;

	// 公共初始化
	inline void init() {
		this->start = new N_node;
		this->end = new N_node;
	}

	// 重载 NFA 闭包 运算
	// 重载前置++ 成员函数默认会操作当前对象, 因此传参会比友元函数少一个
	// 友元函数只能传入引用或真实对象,不能传入对象指针
	friend NFA* operator ++ (NFA& nfa) {
		if (!nfa.end->has_next(epsilon, *nfa.end))
			nfa.end->add_next_node(epsilon, *nfa.start);

		NFA *new_NFA = new NFA;
		new_NFA->init();
		new_NFA->start->add_next_node(epsilon, *nfa.start);
		new_NFA->start->add_next_node(epsilon, *new_NFA->end);
		nfa.end->add_next_node(epsilon, *new_NFA->end);

		return new_NFA;
	}

	// 重载后置++
	friend NFA* operator ++(NFA& nfa, int) {
		NFA res = nfa;
		++nfa;
		return &res;
	}

	// 重载 NFA 或 运算
	friend NFA* operator | (NFA& nfa1, NFA& nfa2) {
		NFA *new_NFA = new NFA; // start end 指针默认为nullptr
		new_NFA->init();

		new_NFA->start->add_next_node(epsilon, *nfa1.start);
		new_NFA->start->add_next_node(epsilon, *nfa2.start);
		nfa1.end->add_next_node(epsilon, *(new_NFA->end));
		nfa2.end->add_next_node(epsilon, *(new_NFA->end));

		return new_NFA;
	}

	// 重载 NFA 连接运算
	// 注意, 连接运算不能单纯将两个NFA用ε连接, 这是不等价的
	// 必须将第二NFA的start结点用第一个的end结点替换掉
	friend NFA* operator + (NFA& nfa1, NFA& nfa2) {
		NFA *new_NFA = new NFA;  // start end 指针默认为nullptr
		N_node *n1 = nfa1.start, *n2 = nfa2.end;
		new_NFA->start = n1;
		new_NFA->end = n2;
		N_node *n1end = nfa1.end, *n2start = nfa2.start; // 替换
		for (auto p : n2start->next) {
			for (auto n_ptr : *p.second) {
				nfa1.end->add_next_node(p.first, *n_ptr);
			}
		}
		delete n2start;
		nfa2.start = nullptr;
		return new_NFA;
	}

	friend ostream& operator << (ostream& out, NFA& nfa) {
		Table table;
		table.set_table_name(string("正规式 ") + nfa.regexp + " NFA表");
		// 表头
		vector<Cell> head;
		Cell c1, c2, c3, c4;
		c1.insert_line(string("当前结点id "));
		c2.insert_line(string("可接受的字符"));
		c3.insert_line(string("可达的下一个结点id"));
		c4.insert_line(string("备注"));
		head.push_back(c1); head.push_back(c2); head.push_back(c3); head.push_back(c4);
		table.set_table_head(head);

		// bfs 遍历结点, 同时构建输出表
		// 可能有自环, 因此用 set 查重
		// buff 用于非字符串向字符串的转换
		queue<N_node*> bfs_q;
		set<int> visited;
		char buff[101];

		bfs_q.push(nfa.start);
		visited.insert(nfa.start->id);

		while (!bfs_q.empty()) {
			// 获取一个新的结点同时创建一行
			N_node *node = bfs_q.front(); bfs_q.pop();
			if (node->is_end() && node->has_next() == false) {
				vector<Cell> line;
				Cell cur_id, ac_char, next_nodes, comment; //当前结点 id, 可接受字符, 下一个可达结点, 备注
				sprintf_s(buff, 100, "%d", node->id);
				cur_id.insert_line(string(buff));
				ac_char.insert_line(string("无"));
				next_nodes.insert_line(string("无"));
				comment.insert_line(string("终结结点"));
				line.push_back(cur_id); line.push_back(ac_char); line.push_back(next_nodes); line.push_back(comment);
				table.insert_row(line);
				continue;
			}
			// 如果不是结束结点, 则每个字符对应的次态结点都是一行
			for (auto key_value : node->next) {
				// 构建一行, 注意和表头相关联
				vector<Cell> line;
				Cell cur_id, ac_char, next_nodes, comment; //当前结点 id, 可接受字符, 下一个可达结点, 备注

				if (node->is_end())comment.insert_line(string("终结结点"));
				else if (node->is_start())comment.insert_line(string("起始结点"));

				sprintf_s(buff, 100, "%d", node->id);
				cur_id.insert_line(string(buff));
				ac_char.insert_line(string({ key_value.first, '\0' }));
				// 遍历可达结点
				string next_id;
				for (auto next_node : (*key_value.second)) {
					sprintf_s(buff, 100, "%d, ", next_node->id);
					next_id += buff;
					// 将结点加入队列和已访问表
					if (visited.count(next_node->id) == 0) {
						bfs_q.push(next_node);
						visited.insert(next_node->id);
					}
				}
				// 将内容加入行
				next_nodes.insert_line(next_id);
				line.push_back(cur_id); line.push_back(ac_char); line.push_back(next_nodes); line.push_back(comment);
				table.insert_row(line);
			}

		}
		// 输出表项
		out << endl << table << endl;
		return out;
	}

};

stack<NFA*> NFA::NFA_destroy;
set<N_node*> NFA::vis_N_node_destroy;

NFA::NFA()
{
	NFA::NFA_destroy.push(this); // 加入全局队列防止内存泄漏
}

NFA::NFA(char ch)
{
	NFA::NFA_destroy.push(this);
	init();
	this->start->add_next_node(ch, *(this->end));
}

NFA::~NFA()
{
	queue<N_node*> bfs_q;
	set<N_node*> &vis_node = NFA::vis_N_node_destroy; // 这里原本希望用id来标识结点, 但是出现自环时, 已释放的结点的id会变化, 因此只能直接用地址来标识结点

	if (this->start == nullptr || vis_node.count(this->start)) return;// 用bfs的方式释放内存, 所有结点在第一次bfs时已释放, 因此用 set 查重
		

	bfs_q.push(this->start);
	vis_node.insert(this->start);
	while (!bfs_q.empty()) {
		N_node* nptr = bfs_q.front(); bfs_q.pop();
		for (auto p : nptr->next) {
			for (auto node : *(p.second)) {
				if (!vis_node.count(node)) {
					bfs_q.push(node);
					vis_node.insert(node);
				}
			}
		}
		delete nptr;
	}
}


void destroy_nfa(NFA* nfa_ptr) {
	nfa_ptr->~NFA();
}

