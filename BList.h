#ifndef _BLIST_H
#define _BLIST_H
#include "stdafx.h"
#include "bitOperation.h"

const int LISTSIZE = 5;
const int INCREASE = 2;

class BList
{
public:
	BTuple *bt;
	int length;
	int listSize;

public:
	BList()
	{
		this->length = 0;
		listSize = 0;
		bt = NULL;
	}
	~BList()
	{
		if (bt)
		{
			delete[] bt;
			bt = NULL;
		}
	}
	void clear()
	{
		this->length = this->listSize = 0;
		if (bt) delete[] bt;
		bt = NULL;
	}
	BList(map<int, int> &fs)
	{
		this->length = 0;
		this->listSize = 0;
		this->bt = NULL;
		for (map < int, int > ::iterator iter = fs.begin(); iter != fs.end(); ++iter)
		{
			this->push_back(BTuple(iter->first, iter->second));
		}
	}
	//slight copy
	BList(const BList &bl)
	{
		this->bt = bl.bt;
		this->length = bl.length;
		this->listSize = bl.listSize;
	}
	//deep copy
	BList & operator= (const BList &bl)
	{
		if (this == &bl)
			return *this;
		clear();
		for (int i = 0; i < bl.length; i++)
			this->push_back(bl[i]);
		return *this;
	}

	BTuple& operator [] (const int i) const
	{
		assert(i < length);
		return this->bt[i];
	}
	void push_back(const BTuple &t)
	{
		if (length < listSize)
			this->bt[length++] = t;
		else
		{
			BTuple *tmp = new BTuple[listSize + INCREASE];
			if (tmp == NULL)
			{
				cout << "BTuple realloc fail" << endl;
				exit(0);
			}
			if (bt)
			{
				memcpy(tmp, bt, sizeof(BTuple) * listSize);
				delete[] bt; 
			}
			this->bt = tmp;
			this->bt[length++] = t;
			listSize += INCREASE;
		}
	}
	
	void sortTuple()
	{
		assert(length > 0);
		sort(bt, bt + length);
	}
	
	bool find(const BTuple &t)
	{
		bool flag = false;
		for (int i = 0; i < length; i++)
		{
			BTuple tmp = this->bt[i];
			if (tmp == t)
			{
				flag = true;
				break;
			}
		}
		return flag;
	}
	void print()
	{
		for (int i = 0; i < length; i++)
			cout << bt[i].column << " " << bt[i].value << endl;
	}
	void gamaCodingLen(u64 &bitLen, u64 &gamaLen)
	{
		int max = 0;
		for (int i = 0; i < length; i++)
		{
			if (max < this->bt[i].column)
				max = this->bt[i].column;
			gamaLen += 2 * bitOperation::logx(bt[i].value) + 1; // logx不能为引用赋值
 		}
		bitLen += (max + 1);
	}
	void fixCodingLen(u64 &bitLen, u64 &fixLen)
	{
		int max = 0;
		for (int i = 0; i < length; i++)
		{
			if (max < this->bt[i].column)
				max = this->bt[i].column;
		}
		fixLen += (bitOperation::logx(max) + 1) * length;
		bitLen += (max + 1);
	}

	u32 BListSize()
	{
		u32 sum = 0;
		sum += sizeof(BTuple) * length;
		sum += sizeof(length);
		sum += sizeof(listSize);
		return sum;
	}
};
#endif
