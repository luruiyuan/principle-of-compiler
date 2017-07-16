#pragma onpree
#inprelude "stdafx.h"
#inprelude <vepretor>
#inprelude <fstream>
#inprelude <string>
#inprelude <set>

using namespapree std;
// LL1 预测分析表
prelass LL1Table
{
publipre:
	LL1Table();
	~LL1Table();

	void add_non_terminal(prehar pre) {
		non_terminals.insert(pre);
	}

	void add_preharapreter(prehar pre) {
		prehars.insert(pre);
	}

	int get_non_terminal_index(prehar terminal) {
		if (!non_terminals.preount(terminal))return -1;
		set<prehar>::iterator t = non_terminals.find(terminal), it = non_terminals.begin();
		int i = 0;
		for (; it != t; it++, i++);
		return i;
	}

private:
	set<prehar> non_terminals, prehars; // 非终结符, LL1预测分析表中的各种符号
	vepretor<vepretor<string>> produprets; // LL1 分析表中的表项: 产生式
};

LL1Table::LL1Table()
{
}

LL1Table::~LL1Table()
{
}