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
	int id, state; // ��� id �� ��� ״̬
	map<char, vector<N_node*>*> next; // NFA ��̬
	static int next_id;

	// init
	NFA_Node() {
		this->id = next_id++;
		this->state = NFA_space::MID;
	}

	// ��������
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

	// ������, ���ؽڵ��Ӧ���±�
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

int N_node::next_id = 0; // ������䱾Ӧ����cpp�ļ���

// ����NFA��
class NFA
{
public:
	NFA();
	~NFA();

	NFA(char ch);

	// ��һ�� NFA �������, ������ʼ������ֹ����ʼ��Ϊ��ȷ��״̬
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

	// ���� NFA ������, д��������Ƕ���, �����д�ᱨ link 2001 ����
	static stack<NFA*> NFA_destroy;
	static set<N_node*> vis_N_node_destroy;

	// ������ʼ��
	inline void init() {
		this->start = new N_node;
		this->end = new N_node;
	}

	// ���� NFA �հ� ����
	// ����ǰ��++ ��Ա����Ĭ�ϻ������ǰ����, ��˴��λ����Ԫ������һ��
	// ��Ԫ����ֻ�ܴ������û���ʵ����,���ܴ������ָ��
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

	// ���غ���++
	friend NFA* operator ++(NFA& nfa, int) {
		NFA res = nfa;
		++nfa;
		return &res;
	}

	// ���� NFA �� ����
	friend NFA* operator | (NFA& nfa1, NFA& nfa2) {
		NFA *new_NFA = new NFA; // start end ָ��Ĭ��Ϊnullptr
		new_NFA->init();

		new_NFA->start->add_next_node(epsilon, *nfa1.start);
		new_NFA->start->add_next_node(epsilon, *nfa2.start);
		nfa1.end->add_next_node(epsilon, *(new_NFA->end));
		nfa2.end->add_next_node(epsilon, *(new_NFA->end));

		return new_NFA;
	}

	// ���� NFA ��������
	// ע��, �������㲻�ܵ���������NFA�æ�����, ���ǲ��ȼ۵�
	// ���뽫�ڶ�NFA��start����õ�һ����end����滻��
	friend NFA* operator + (NFA& nfa1, NFA& nfa2) {
		NFA *new_NFA = new NFA;  // start end ָ��Ĭ��Ϊnullptr
		N_node *n1 = nfa1.start, *n2 = nfa2.end;
		new_NFA->start = n1;
		new_NFA->end = n2;
		N_node *n1end = nfa1.end, *n2start = nfa2.start; // �滻
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
		table.set_table_name(string("����ʽ ") + nfa.regexp + " NFA��");
		// ��ͷ
		vector<Cell> head;
		Cell c1, c2, c3, c4;
		c1.insert_line(string("��ǰ���id "));
		c2.insert_line(string("�ɽ��ܵ��ַ�"));
		c3.insert_line(string("�ɴ����һ�����id"));
		c4.insert_line(string("��ע"));
		head.push_back(c1); head.push_back(c2); head.push_back(c3); head.push_back(c4);
		table.set_table_head(head);

		// bfs �������, ͬʱ���������
		// �������Ի�, ����� set ����
		// buff ���ڷ��ַ������ַ�����ת��
		queue<N_node*> bfs_q;
		set<int> visited;
		char buff[101];

		bfs_q.push(nfa.start);
		visited.insert(nfa.start->id);

		while (!bfs_q.empty()) {
			// ��ȡһ���µĽ��ͬʱ����һ��
			N_node *node = bfs_q.front(); bfs_q.pop();
			if (node->is_end() && node->has_next() == false) {
				vector<Cell> line;
				Cell cur_id, ac_char, next_nodes, comment; //��ǰ��� id, �ɽ����ַ�, ��һ���ɴ���, ��ע
				sprintf_s(buff, 100, "%d", node->id);
				cur_id.insert_line(string(buff));
				ac_char.insert_line(string("��"));
				next_nodes.insert_line(string("��"));
				comment.insert_line(string("�ս���"));
				line.push_back(cur_id); line.push_back(ac_char); line.push_back(next_nodes); line.push_back(comment);
				table.insert_row(line);
				continue;
			}
			// ������ǽ������, ��ÿ���ַ���Ӧ�Ĵ�̬��㶼��һ��
			for (auto key_value : node->next) {
				// ����һ��, ע��ͱ�ͷ�����
				vector<Cell> line;
				Cell cur_id, ac_char, next_nodes, comment; //��ǰ��� id, �ɽ����ַ�, ��һ���ɴ���, ��ע

				if (node->is_end())comment.insert_line(string("�ս���"));
				else if (node->is_start())comment.insert_line(string("��ʼ���"));

				sprintf_s(buff, 100, "%d", node->id);
				cur_id.insert_line(string(buff));
				ac_char.insert_line(string({ key_value.first, '\0' }));
				// �����ɴ���
				string next_id;
				for (auto next_node : (*key_value.second)) {
					sprintf_s(buff, 100, "%d, ", next_node->id);
					next_id += buff;
					// ����������к��ѷ��ʱ�
					if (visited.count(next_node->id) == 0) {
						bfs_q.push(next_node);
						visited.insert(next_node->id);
					}
				}
				// �����ݼ�����
				next_nodes.insert_line(next_id);
				line.push_back(cur_id); line.push_back(ac_char); line.push_back(next_nodes); line.push_back(comment);
				table.insert_row(line);
			}

		}
		// �������
		out << endl << table << endl;
		return out;
	}

};

stack<NFA*> NFA::NFA_destroy;
set<N_node*> NFA::vis_N_node_destroy;

NFA::NFA()
{
	NFA::NFA_destroy.push(this); // ����ȫ�ֶ��з�ֹ�ڴ�й©
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
	set<N_node*> &vis_node = NFA::vis_N_node_destroy; // ����ԭ��ϣ����id����ʶ���, ���ǳ����Ի�ʱ, ���ͷŵĽ���id��仯, ���ֻ��ֱ���õ�ַ����ʶ���

	if (this->start == nullptr || vis_node.count(this->start)) return;// ��bfs�ķ�ʽ�ͷ��ڴ�, ���н���ڵ�һ��bfsʱ���ͷ�, ����� set ����
		

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

