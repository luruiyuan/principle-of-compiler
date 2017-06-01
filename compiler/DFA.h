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
	set<int> NFA_node_id;

	DFA_node(int id) {
		this->id = id;
		status = DFA_space::MID;
	}

	~DFA_node() {
		this->next.clear();
		this->NFA_node_id.clear();
	}

	bool has_next(char ch) {
		return next.count(ch) > 0;
	}

	bool is_mid() {
		return status = DFA_space::MID;
	}

	bool is_end() {
		return status = DFA_space::END;
	}

	bool is_start() {
		return status = DEAD;
	}

	bool is_dead() {
		return status = DEAD;
	}

	int add_next(char ch, D_node *node) {
		if (has_next(ch))return -1;
		next[ch] = node;
		return 0;
	}

	D_node *get_next_node(char ch) {
		if (has_next(ch))return next[ch];
		return nullptr;
	}

};

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

private:
	string regexp;
	D_node *start;
	set<set<int>> nfa_node_set; // 集合中每个结点是NFA的结点id, 用于NFA转DFA时查重
	map<int, N_node*> id2N_node; // 通过结点id查找该DFA中的结点, 用于NFA转DFA时遍历当前结点集合
	set<char>* alpha_table;

	static queue<DFA*> dfa_destroy;
	static set<D_node*> vis_D_node;

};

queue<DFA*> DFA::dfa_destroy;
set<D_node*> DFA::vis_D_node;

DFA::DFA()
{
	dfa_destroy.push(this);
}

DFA::~DFA()
{
	delete this->alpha_table;
	if (vis_D_node.count(start) > 0)return;
	queue<D_node*> bfs_q;
	set<D_node*> &vis = vis_D_node;

	bfs_q.push(start);
	vis.insert(start);
	while (!bfs_q.empty()) {
		D_node *dn = bfs_q.front();
		for (auto p : dn->next) {
			if (vis.count(p.second) > 0)continue;
			vis.insert(p.second);
			bfs_q.push(p.second);
		}
		delete dn;
	}
}