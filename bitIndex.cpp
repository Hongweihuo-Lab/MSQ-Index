#include "bitIndex.h"
void bitIndex::encodeGama(vector<int> &value, u64 &offHybrid, u64 &offSample, u64 &offGamma)
{
	for (int i = 0; i < value.size(); i++)
	{
		u64 pos = offSample + i;
		if (pos % blockSize == 0)
		{
			bitOperation::setbit(blockOffset, (pos / blockSize) * sampleFSLength, offHybrid, sampleFSLength);
			bitOperation::setbit(flagRank->bit, (pos / blockSize), 1, 1);
		}
		u64 len = 2 * bitOperation::logx(value[i]) + 1;
		bitOperation::setbit(this->fsuccinct, offHybrid, value[i], len);
		offHybrid += len;
	}
	offSample += value.size();
}
void bitIndex::encodeFix(vector<int> &value, u64 &offHybrid, u64 &offSample, u64 &offWords)
{
	int max = 0;
	for (int i = 0; i < value.size(); i++)
	{
		if (max < value[i])
			max = value[i];
	}
	int fixBitLen = bitOperation::logx(max) + 1;

	for (int i = 0; i < value.size(); i++)
	{
		u64 pos = offSample + i;
		if (pos % blockSize == 0) 
		{
			bitOperation::setbit(blockOffset, (pos / blockSize) * sampleFSLength, offHybrid, sampleFSLength);
			bitOperation::setbit(words, offWords * wordsLength, fixBitLen, wordsLength);
			offWords++;
		}
		bitOperation::setbit(fsuccinct, offHybrid, value[i], fixBitLen);
		offHybrid += fixBitLen;
	}
	offSample += value.size();
}
//true: gamma, false: fix
void bitIndex::encode(vector<BTuple> &vb, u64 &offPos, u64 &offHybrid, u64 &offSample, u64 &offGamma, u64 &offFix)
{
	assert(vb.size() > 0);
	int size = vb.size();
	int maxc = 0, maxv = 0;
	u32 bitlen = 0;
	bool flag;
	vector<int> value;
	for (int i = 0; i < size; i++)
	{
		if (maxc < vb[i].column)
			maxc = vb[i].column;
		if (maxv < vb[i].value)
			maxv = vb[i].value;
		bitOperation::setbit(posRank->bit, (u64)(offPos + vb[i].column), 1, 1);
		bitlen += 2 * bitOperation::logx(vb[i].value) + 1;
		value.push_back(vb[i].value);
	}
	offPos += maxc + 1; 
	if (bitlen < (bitOperation::logx(maxv) + 1) * size)
		this->encodeGama(value, offHybrid, offSample, offGamma);
	else
		this->encodeFix(value, offHybrid, offSample, offFix);
}

