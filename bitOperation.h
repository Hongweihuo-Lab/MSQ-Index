#ifndef _BITOPERATION_H
#define _BITOPERATION_H
#include "global.h"

class bitOperation
{
public:
	static inline void setbit(u64 *bitvec, u64 i, u64 value, u64 len)
	{
		u64 word = i / B64;  
		u64 offset = i % B64;
		u64 j = B64 - offset; 
		if (j >= len)
		{
			value = value << (j - len);
			bitvec[word] += value;
		}
		else 
		{
			u64 tmp = value >> (len - j);
			bitvec[word] += tmp; 
			tmp = value << (B64 - len + j);
			bitvec[word + 1] += tmp;
		}
	}
	
	static inline u64 getbit(const u64 *bitvec, const u64 &i, const int &len)
	{
		register u64 word = i / B64;  
		register u64 offset = i % B64; 
		register u64 j = B64 - offset;
		register u64 tmp;
		if (j >= len) 
		{
			tmp = bitvec[word] << offset;
			tmp = tmp >> (B64 - len);
		}
		else 
		{
			tmp = bitvec[word] << offset;
			tmp = tmp >> (B64 - len);
			tmp += bitvec[word + 1] >> (B64 - (len - j));
		}
		return tmp;
	}
	
	static inline u64 popcount(register u64 r)
	{
		//knuth
		r = ((r & 0xaaaaaaaaaaaaaaaa) >> 1) + (r & 0x5555555555555555);
		r = ((r & 0xcccccccccccccccc) >> 2) + (r & 0x3333333333333333);
		r = ((r >> 4) + r) & 0x0f0f0f0f0f0f0f0f;
		r *= 0x0101010101010101;
		r >>= 64 - 8;
		return r;
		
	}

	static inline u32 logx(u64 num)
	{
		int len = 0;
		while ((num = num / 2) >= 1)
			len++;
		return len;
	}

	static int gamaLen(const vector<int> &vr, int left, int right)
	{
		int sum = 0;
		for (int i = left; i < right; i++)
		{
			sum += 2 * bitOperation::logx(vr[i]) + 1;
		}
		return sum;
	}

	static int fixLen(const vector<int> &vr, int left, int right)
	{
		int sum = 0;
		int max = vr[left];
		for (int i = left + 1; i < right; i++)
		{
			if (max < vr[i])
				max = vr[i];
		}
		return (right-left) * (bitOperation::logx(max) + 1);
	}
	static u32 encodingLen(vector<int> &value, bool &f, u32 &maxValue)
	{
		u32 bitlen = 0;
		int size = value.size();
		int max = 0;
		for (int j = 0; j < size; j++)
		{
			if (max < value[j])
				max = value[j];
			bitlen += 2 * bitOperation::logx(value[j]) + 1;
		}

		if (maxValue < max)
			maxValue = max;
		if (bitlen < (bitOperation::logx(max) + 1) * size)
			f = true;
		else
			f = false;

		bitlen = min(bitlen, (bitOperation::logx(max) + 1) * size);
		return bitlen;
	}
	static bool fixMoreGamma(vector<int> &value)
	{
		u32 bitlen = 0;
		int max = 0;
		u32 size = value.size();
		for (int j = 0; j < size; j++)
		{
			if (max < value[j])
				max = value[j];
			bitlen += 2 * bitOperation::logx(value[j]) + 1;
		}
		if (bitlen < (bitOperation::logx(max) + 1) * size)
			return true;
		else
			return false;
	}
};
#endif
