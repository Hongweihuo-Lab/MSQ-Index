#ifndef _RANKSELECT_H
#define _RANKSELECT_H
#include "bitOperation.h"


class rankSelect
{
public:
	u64 *bit;
	u64 *ST;
	u8 *BT;
	u64 bitlen; 

	u64 sb_size; 
	u64 sb_len; 
	u64 b_len;  
	u64 b_size; 

public:
	rankSelect()
	{ 
		bit = ST = NULL;
		BT = NULL;
		sb_size = sb_len = 0;
		b_len = 8;
	}
	~rankSelect()
	{
		if (ST) delete[] ST;
		if (BT) delete[] BT;
		if (bit) delete[] bit;
	}
	rankSelect(const u64 &len)
	{
		this->bitlen = len;
		this->b_size = 64;
		this->sb_size = RATE * this->b_size;

		sb_len = bitOperation::logx(len) + 1;
		u64 superBlockNum = bitlen / sb_size + (bitlen % sb_size ? 1 : 0);
		u64 superBlockBit = superBlockNum * sb_len;
		u64 superBlockLen = superBlockBit / B64 + (superBlockBit % B64 ? 1 : 0);
		ST = new u64[superBlockLen]; memset(ST, 0, sizeof(u64) * superBlockLen);

		b_len = 8;
		u64 blockNum = bitlen / b_size + (bitlen % b_size ? 1 : 0);
		BT = new u8[blockNum]; memset(BT, 0, sizeof(u8) * blockNum);
		
		u64 bitvec64 = bitlen / B64 + ((bitlen % B64) ? 1 : 0);
		bit = new u64[bitvec64]; memset(bit, 0, sizeof(u64) * bitvec64);

	}
	void initRankTable()
	{
		u64 bitvec64 = bitlen / B64 + ((bitlen % B64) ? 1 : 0);
		u64 addupB = 0;
		u64 blen = 0;
		u64 sblen = 0;
		u64 addupSB = 0;

		for (u64 i = 0; i < bitvec64; i++)
		{
			u64 tmpi = i * 64;
			if (tmpi % sb_size == 0) 
			{
				addupSB += addupB;
				addupB = 0;
				bitOperation::setbit(this->ST, sblen, addupSB, sb_len);
				sblen += sb_len;
			}
			if (tmpi % b_size == 0) 
			{
				BT[blen] = addupB;
				blen++;
			}
			addupB += bitOperation::popcount(bit[i]);
		}
	}

	rankSelect(u64 *bit, u64 len)
	{
		this->bit = bit;
		this->bitlen = len;
		this->b_size = 64;
		this->sb_size = RATE * this->b_size;

		sb_len = bitOperation::logx(len) + 1;
		u64 superBlockNum = bitlen / sb_size + (bitlen % sb_size ? 1 : 0);
		u64 superBlockBit = superBlockNum * sb_len;
		u64 superBlockLen = superBlockBit / B64 + (superBlockBit % B64 ? 1 : 0); 
		ST = new u64[superBlockLen]; memset(ST, 0, sizeof(u64) * superBlockLen);

		b_len = 8;
		u64 blockNum = bitlen / b_size + (bitlen % b_size ? 1 : 0);
		BT = new u8[blockNum]; memset(BT, 0, sizeof(u8) * blockNum);

		initRankTable();
	}
	
public:
	u32 rank(const u64 &i)
	{
		assert(i < bitlen);
		register u64 s_off = i / sb_size;
		register u64 b_off = i / b_size;
		register u64 beginRank = b_off << 6;
	
		register u64 rank_num = bitOperation::getbit(this->ST, sb_len * s_off, sb_len); //SB[s_off]
		rank_num += BT[b_off];
		while (beginRank <= i)
		{

			int left = i - beginRank + 1;
			if (left < B64)   
			{
				u64 temp = bitOperation::getbit(this->bit, beginRank, left);
				rank_num += bitOperation::popcount(temp);
				break;
			}
			else
			{
				rank_num += bitOperation::popcount(this->bit[beginRank / B64]);
				beginRank += B64;
			}
		}
		return rank_num;
	}
	u64 byte_count() //bytes
	{
		u64 sum = 0;
		u64 superBlockNum = bitlen / sb_size + (bitlen % sb_size ? 1 : 0);
		u64 superBlockBit = superBlockNum * sb_len;
		u64 superBlockLen = superBlockBit / B64 + (superBlockBit % B64 ? 1 : 0);
		sum += sizeof(u64) * superBlockLen;

		u64 blockNum = bitlen / b_size + (bitlen % b_size ? 1 : 0);
		sum += sizeof(u8) * blockNum;
		
		u64 bitvec64 = bitlen / B64 + ((bitlen % B64) ? 1 : 0);
		sum += sizeof(u64) * bitvec64;
		return sum;
	}
public:
	u32 rankValueOne(const u64 &i) //[0, i]
	{
		return this->rank(i);
	}
	u32 rankValueZero(const u64 &i)// [0, i]
	{
		return i + 1 - rank(i);
	}
	bool checkOne(const u64 &j)
	{
		register int word = j / B64;
		register int offset = j % B64;
		if (j >  bitlen) return false;  //the index begin with 0,...
		register u64 temp = bit[word] >> (B64 - offset - 1);
		return temp % 2;
	}
};

#endif