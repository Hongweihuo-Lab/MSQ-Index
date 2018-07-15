#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "stdafx.h"

struct BTuple
{
	int column;
	int value;
	BTuple(){}
	BTuple(int c, int v)
	{
		column = c;
		value = v;
	}
	bool operator< (const BTuple &t) const
	{
		if (column < t.column) return true;
		else if (column > t.column) return false;
		else
			return value < t.value;
	}
	bool operator== (const BTuple &t) const
	{
		if (column == t.column) return true;
		else
			return false;
	}
	bool operator>(const BTuple &t) const
	{
		if (column > t.column) return true;
		else
		{
			if (column < t.column) return false;
			else
				return value > t.value;
		}
	}
	BTuple(const BTuple &t)
	{
		this->column = t.column;
		this->value = t.value;
	}
	BTuple & operator = (const BTuple &t)
	{
		if (this == &t)
			return *this;
		this->column = t.column;
		this->value = t.value;
		return *this;
	}
};
struct twoTuple
{
	int row;
	int value;
	bool operator< (const twoTuple &t) const
	{
		return row < t.row || (row == t.row && value <= t.value);
	}
	bool operator= (const twoTuple &t) const
	{
		return row == t.row;
	}
	twoTuple(int r, int v) : row(r), value(v) { }
	twoTuple(){}
};

struct sTuple
{
	u64 bitLen;
	u64 codeLen;
	u64 codeNumber;
	u64 gamaCounts;
	u64 fixCounts;
	u64 maxValue;
	sTuple()
	{
		bitLen = codeLen = codeNumber = gamaCounts = fixCounts = maxValue = 0;
	}
	sTuple(const u64 &b, const u64 &c, const u64 &n, const u64 &g, const u64 &f, const u64 &m)
		: bitLen(b), codeLen(c), codeNumber(n), gamaCounts(g), fixCounts(f), maxValue(m) {}
};

struct stringInt
{
	string str;
	int degree;
	stringInt(string s, int d) : str(s), degree(d) { }
	stringInt(){}
};

typedef struct bitTreeOffset
{
	u64 offPos;
	u64 offHybrid;
	u64 offSample;
	u64 offGamma;
	u64 offWords;
	bitTreeOffset(u64 p, u64 h, u64 s, u64 g, u64 w) :
		offPos(p), offHybrid(h), offSample(s), offGamma(g), offWords(w)
	{}
};

//with testing 
typedef struct Succinct
{
	u64 I_X;
	u64 B_X;
	u64 V_X;
	u64 T_X;
	Succinct(){}
	Succinct(u64 ix, u64 bx, u64 vx, u64 tx) :
		I_X(ix), B_X(bx), V_X(vx), T_X(tx){}

};

#define B64 64
#define B32 32
#define B16 16
#define MAXNODES 63
#define MINNODES 32

#define RATE 4
extern int BBS;
extern int EBS;
extern int VBS;

static int position = 1;
const int MAX = 200000;

extern u64 g_vertex;
extern u64 g_edge;
extern u64 g_vertex_begin;
extern u64 g_vertex_end;
extern u64 g_edge_beign;
extern u64 g_edge_end;
extern u64 g_branch_begin;
extern u64 g_branch_end;
extern BTuple g_branch_tuple[1024];
extern int g_branch_count;
extern int g_degree[1024];
extern int g_count;
extern int q_degree[1024];
extern int q_count;

extern Succinct SD;
extern Succinct SV;
extern Succinct SE; 

extern u64 Ma, Mb, Mc;
extern map<int, int> MD;
extern map<int, int> ML; 


#endif