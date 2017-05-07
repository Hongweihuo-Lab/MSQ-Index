#ifndef _FIX_CODE_H
#define _FIX_CODE_H
#include "stdafx.h"
#include "bitOperation.h"
#include "common.h"
class fixCode
{
public:
	u64 *bit;
	u32 words;
	u32 size;
public:
	fixCode(){ bit = NULL; words = 0; size = 0; }
	fixCode(const u64 max, const u64 len)
	{
		this->words = bitOperation::logx(max) + 1;
		u64 bitLen = len * this->words;
		u64 bitArrayLen = bitLen / B64 + ((bitLen % B64 == 0) ? 0 : 1);
		bit = new u64[bitArrayLen]; assert(bit); memset(this->bit, 0, sizeof(u64) * bitArrayLen);
		this->size = sizeof(u64) * bitArrayLen;
	}
	u32 fixCodeSize(){ return size + sizeof(void *) + sizeof(words); }
	void getValue(const u64 &idx, u64 &begin, u64 &end)
	{
		u64 off = 2 * idx * this->words;
		begin = bitOperation::getbit(this->bit, off, this->words);
		end = bitOperation::getbit(this->bit, off + this->words, this->words);
	}
public:
	~fixCode(){ if (bit) delete[] bit; bit = NULL; }

};
#endif