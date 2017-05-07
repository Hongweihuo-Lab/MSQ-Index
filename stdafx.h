// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//
#pragma once
#pragma warning( disable : 4996) 

#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <set>
#include <ctime>
#include <queue>
#include <functional>
#include <algorithm>
#include <random>
#include <list>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/time.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <time.h>
#define  fseeko _fseeki64
#define stat64 _stati64
#else
#include <sys/types.h>
#include <bitset>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define _stati64 stat64
#define _fseeki64 fseeko
#define _FILE_OFFSET_BITS 64   
#define ASSERT assert
typedef unsigned char uchar;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int uint;

#define B64 64
#define B32 32
#define B16 16
#define MAXNODES 63
#define MINNODES 32
//#define BBS 16
//#define VBS 16
//#define EBS 16
#define RATE 4
static int BBS = 16;
static int EBS = 16;
static int VBS = 16;

static int position = 1;
const int MAX = 200000;
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
