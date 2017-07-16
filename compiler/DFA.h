#pragma once
#include "stdafx.h"
#include <map>
#include <fstream>
#include <set>
#include <queue>

#include "table.h"
using namespace std;
using namespace DFA_space;

typedef struct DFA_node D_node;
struct DFA_node
{
	int id, status;
	map<char, D_node*> next;
	set<N_node*> *NFA_nodes;

	DFA_node(int id) {
		this->id = id;
		status = DFA_space::MID;
	}

	DFA_node(int id, int status, set<N_node*> *nfa_nodes_ptr) {
		this->id = id;
		this->status = status;
		NFA_nodes = nfa_nodes_ptr;
	}

	~DFA_node() {
		this->next.clear();
		if (NFA_nodes != nullptr) {
			this->NFA_nodes->clear();
			delete NFA_nodes;
		}

	}

	void add_NFA_node(N_node *node) {
		this->NFA_nodes->insert(node);
	}

	void set_nfa_nodes(set<N_node*> *s) {
		this->NFA_nodes = s;
	}

	bool has_next(char ch) {
		return next.count(ch) > 0;
	}

	bool has_next() {
		return next.size() > 0;
	}

	bool is_mid() {
		return status & (1 << DFA_space::MID);
	}

	bool is_end() {
		return status & (1 << DFA_space::END);
	}

	bool is_start() {
		return status & (1 << DFA_space::START);
	}

	bool is_dead() {
		return status &(1 << DFA_space::DEAD);
	}

	// 后添加的边会覆盖前面的边, 这里是需要覆盖的
	int add_next(char ch, D_node *node) {
		next[ch] = node;
		return 0;
	}

	D_node *get_next_node(char ch) {
		if (has_next(ch))return next[ch];
		return nullptr;
	}

};

//////////////////////////////////////////////////////////////////////////

class DFA
{
public:
	DFA();
	~DFA();

	static void destroy_all_dfa() {
		vis_D_node.clear();
		while (!dfa_destroy.empty()) {
			DFA *dfa = dfa_destroy.front(); dfa_destroy.pop();
			delete dfa;
		}
	}

	void set_regexp(string& reg) {
		this->regexp = reg;
	}

	string& get_regexp() {
		return this->regexp;
	}

	D_node *get_start_node() {
		return this->start;
	}

	void set_start_node(D_node *node) {
		this->start = node;
	}

	void set_alpha_table(set<char>* alpha_table) {
		this->alpha_table = alpha_table;
	}

	set<char> *get_alpha_table() {
		return this->alpha_table;
	}

	int get_next_id() {
		return next_id;
	}

	void update_next_id() {
		this->next_id++;
	}

	bool has_nfa_node_set(set<N_node*> *node_set) {
		return this->nfa_node_set.count(*node_set) > 0;
	}

	// 添加结点
	void add_nfa_node_set(set<N_node*> *node_set) {
		if (has_nfa_node_set(node_set))return;
		this->nfa_node_set.insert(*node_set);
	}

	void add_map_set2NFA_node_addr(set<N_node*> *s, D_node *d) {
		set2N_node[*s] = d;
	}

	D_node *get_D_node_addr_by_set(set<N_node*> *s) {
		if (this->set2N_node.count(*s) == 0)return nullptr;
		return set2N_node[*s];
	}

	void set_simplyfied(bool is_simplyfied) {
		this->is_simplified = is_simplyfied;
	}

	bool has_simplified() {
		return this->is_simplified;
	}
	// 销毁当前DFA的结点但不析构当前DFA
	// 用于DFA化简
	void destroy_cur_dfa_nodes() {
		if (this->start == nullptr)return;
		queue<D_node*> bfs_q;
		set<D_node*> &vis = vis_D_node;
		bfs_q.push(start);
		vis.insert(start);
		while (!bfs_q.empty()) {
			D_node *dn = bfs_q.front(); bfs_q.pop();
			for (auto p : dn->next) {
				if (vis.count(p.second) > 0)continue;
				vis.insert(p.second);
				bfs_q.push(p.second);
			}
			delete dn;
		}
	}

	// 将DFA的next_id属性初始化, 用于化简时构建新的DFA时的结点id初始化
	void init_next_id() {
		this->next_id = 0;
	}

	bool is_acceptable(string &s) {
		if (this->next_id == 0 || this->start == nullptr) {
			cout << "DFA尚未初始化, 判定失败" << endl;
			return false;
		}
		if (s.length() == 0) {
			cout << "传入的字符串为空, 判定失败" << endl;
			return false;
		}
		// 开始遍历DFA
		D_node *cur_node = this->start;
		for (auto cur : s) {
			if (!cur_node->has_next(cur))return false;
			cur_node = cur_node->get_next_node(cur);
		}
		return cur_node->is_end();
	}

private:
	int next_id;
	bool is_simplified; // 是否化简过

	string regexp;
	D_node *start;

	set<set<N_node*>> nfa_node_set; // 集合中每个结点是NFA的结点id, 用于NFA转DFA时查重
	map<set<N_node*>, D_node*> set2N_node; // 通过结点id查找该DFA中的结点, 用于NFA转DFA时遍历当前结点集合
	set<char>* alpha_table;

	static queue<DFA*> dfa_destroy;
	static set<D_node*> vis_D_node;

	friend ostream& operator << (ostream &out, DFA &dfa) {
		Table t;
		string sufix = "化简的 " + dfa.get_regexp() + " DFA表";
		const string arrow = "---> ";
		string t_name = dfa.is_simplified ? string("已") + sufix : string("未") + sufix;
		t.set_table_name(t_name);
		// 表头
		vector<Cell> head;
		Cell c1, c2, c3, c4;
		c1.insert_line(string("当前结点id"));
		if (!dfa.is_simplified)c2.insert_line(string("当前结点包含的NFA结点id")); // 如果未化简, 则添加NFA结点id, 化简后可能不具有对应关系
		c3.insert_line(string("可接受的字符 " + arrow + "次态DFA结点id"), 40);
		c4.insert_line(string("备注"));
		head.push_back(c1); if (!dfa.is_simplified)head.push_back(c2);
		head.push_back(c3); head.push_back(c4);
		t.set_table_head(head);

		// 开始 bfs
		queue<D_node*> bfs_q;
		set<D_node*> visited;
		char buff[101];

		bfs_q.push(dfa.start);
		visited.insert(dfa.start);

		while (!bfs_q.empty()) {
			D_node *node = bfs_q.front(); bfs_q.pop();
			vector<Cell> line;
			Cell cur_id, n_id, ac_ch, comment;//当前id, NFA结点集, 可接受字符与次态DFA结点, 备注
			// 加入当前结点 id
			sprintf_s(buff, 100, "%d", node->id);
			cur_id.insert_line(string(buff));
			if (!dfa.is_simplified) {
				// 如果未化简, 则将当前DFA结点包含的 NFA 结点 id 加入输出表项, 尝试换行和输出
				string nfa_id;
				int cnt = 0;
				for (auto nfa_n : *node->NFA_nodes) {
					sprintf_s(buff, 100, "%d, ", nfa_n->id);
					nfa_id += buff;
					if ((++cnt) % 5 == 0) {
						n_id.insert_line(nfa_id);
						nfa_id = "";
					}
				}
				if (nfa_id != "")n_id.insert_line(nfa_id);
			}
			// 加入备注, 注意 DFA中存在闭包, 因此一个结点可能既是起始结点又是终结结点
			if (node->is_start() && node->is_end())comment.insert_line(string("起始结点|终结结点"));
			else if (node->is_start())comment.insert_line(string("起始结点"));
			else if (node->is_end())comment.insert_line(string("终结结点"));
			// 将次态DFA结点加入队列和输出表项
			if (node->has_next()) {
				for (auto p : node->next) {
					string s({ p.first ,'\0' });
					sprintf_s(buff, 100, "%d", p.second->id);
					s += arrow + buff;
					ac_ch.insert_line(s);
					// 如果未访问过, 加入队列
					if (!visited.count(p.second)) {
						visited.insert(p.second);
						bfs_q.push(p.second);
					}
				}
			}
			else {
				string s("无");
				ac_ch.insert_line(s);
			}
			line.push_back(cur_id);
			if (!dfa.is_simplified)line.push_back(n_id);
			line.push_back(ac_ch); line.push_back(comment);
			t.insert_row(line);
		}
		out << t << endl;
		return out;
	}


};

queue<DFA*> DFA::dfa_destroy;
set<D_node*> DFA::vis_D_node;

DFA::DFA()
{
	dfa_destroy.push(this);
	this->next_id = 0;
	is_simplified = false;
}

DFA::~DFA()
{
	delete this->alpha_table;
	this->destroy_cur_dfa_nodes();
}