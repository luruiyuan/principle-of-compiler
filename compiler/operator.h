#pragma once
#include "stdafx.h"
#include <string>
#include <set>
#include <queue>


const char link_operator = '-';
const char link[2] = { link_operator,'\0' };
const string link_op_str = string(link);
const set<char> op_set = { '*',link_operator,'|' };

const char epsilon = '#';

namespace NFA_space {

	enum Node_State {
		START,// 起始结点
		MID,  // 中间结点
		END,  // 终结结点
	};

}

namespace DFA_space {

	enum DFA_Status
	{
		START,
		MID,
		END,
		DEAD,
	};

}

enum priority
{
	ADD,
	MUL,
	END_REGEXP
};



enum lookup_table {
	OR,
	LINK,
	CLOUSRE,
};