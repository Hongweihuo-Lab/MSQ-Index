#ifndef _RANKINDEX_H
#define _RANKINDEX_H
#include "stdafx.h"
#include "bitOperation.h"
#include "rank9sel.h"
#include "BList.h"


class rankIndex
{
public:
	u64 *bit; 
	u64 bitlen; 
	rank9sel *rank; 

public:
	rankIndex()
	{
		bit = NULL;  rank = NULL;
		bitlen = 0;
	}
	rankIndex(const u64 &len)
	{
		bitlen = len;
		int arraylen = bitlen / B64 + ((bitlen % B64) ? 1 : 0);
		bit = new u64[arraylen];
		memset(bit, 0, sizeof(u64) * arraylen);
		assert(bit);
	}
	rankIndex(u64 *bp, u64 len)
	{
		bit = bp;
		bitlen = len;
		rank = new rank9sel(bp, bitlen + 1);
	}
	rankIndex(const rankIndex &rank) //deep copy
	{
		this->bitlen = rank.bitlen;
		int arraylen = bitlen / B64 + ((bitlen % 64) ? 1 : 0);
		this->bit = new u64[arraylen];
		memcpy(this->bit, rank.bit, this->bitlen);
		this->rank = new rank9sel(this->bit, this->bitlen + 1);
	}

	bool checkOne(u64 j)
	{
		register int word = j / B64;
		register int offset = j % B64;
		if (j >  bitlen) return false;  //the index begin with 0,...
		register u64 temp = bit[word] >> (B64 - offset - 1);
		return temp % 2;
	}
	
	int rankValueOne(u64 i)
	{
		return this->rank->rank(i + 1); 
	}
	int rankValueZero(u64 i)
	{
		assert(i <= bitlen);
		return (i + 1) - this->rankValueOne(i);
	}
	u32 rankIndexSize()
	{
		u32 sum = 0;
		sum += rank->bit_count() / 8;
		sum += bitlen/8;
		sum += sizeof(void *) * 2;
		sum += sizeof(u64);
		return sum;
	}
	~rankIndex()
	{
		if (bit) delete[] bit;
		if (rank) delete rank;
	}

	
	void test()
	{
		u64 *bit = new u64[2];
		bit[0] = 0xf9f9; bitset<64> tmp(bit[0]);  cout << tmp << endl;
		bit[1] = 0x1fffffff; bitset<64> tt(bit[1]); cout << tt << endl;
		rankIndex *r = new rankIndex(bit, 129);
		int v4 = r->rankValueZero(63);
	}
};
#endif