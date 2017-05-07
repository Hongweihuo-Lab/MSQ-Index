#ifndef _BITINDEX_H
#define _BITINDEX_H
#include "bitmap.h"
#include "BList.h"
#include "rankSelect.h"
#include "common.h"

class bitIndex
{
public:
	bitmap *bm;      
	int blockSize;  
	u64 integerLen;  
	u64 indexSize;  

	rankSelect *posRank;  
	rankSelect *flagRank; 
	 
	u64 *fsuccinct; 
	u64 fLen; 
	
	u64 *blockOffset; 
	u64 sampleFSLength; 

	u64 *words; 
	u64 wordsLength; 

public:
	bitIndex()
	{
		bm = NULL;
		posRank = flagRank = NULL;
		fsuccinct = blockOffset = NULL;
		words =  0;
		fLen = sampleFSLength = wordsLength = 0;
		blockSize = 32;
		indexSize = 0;
	}
	bitIndex(const sTuple &st, const int bs)
	{
		this->blockSize = bs;
		this->integerLen = st.codeNumber;
		this->fLen = st.codeLen;
		this->indexSize = 0;

		u64 bitLen = st.bitLen;
		bm = bitmap::getInstance();

		this->posRank = new rankSelect(bitLen); this->indexSize += posRank->byte_count();

		u64 flagArrayLen = bitLen / bs + (bitLen % bs ? 1 : 0);
		this->flagRank = new rankSelect(flagArrayLen);  this->indexSize += flagRank->byte_count();

		u64 codeArrayLen = fLen / B64 + (fLen % B64 ? 1 : 0);
		this->fsuccinct = new u64[codeArrayLen]; memset(fsuccinct, 0, sizeof(u64) * codeArrayLen); this->indexSize += codeArrayLen * sizeof(u64);

		sampleFSLength = bitOperation::logx(fLen) + 1;
		//sampleFSLength = 32;
		u64 sampleArrayNum = integerLen / blockSize + (integerLen % blockSize ? 1 : 0);
		u64 sampleArrayBitLen = sampleFSLength * sampleArrayNum;
		u64 sampleArrayLen = sampleArrayBitLen / B64 + (sampleArrayBitLen % B64 ? 1 : 0);
		this->blockOffset = new u64[sampleArrayLen]; memset(blockOffset, 0, sizeof(u64) * sampleArrayLen); this->indexSize += sampleArrayLen * sizeof(u64);

		u64 gamaCounts = st.gamaCounts; u64 wordsCount = st.fixCounts; u32 maxValue = st.maxValue;
		wordsLength = bitOperation::logx(maxValue) + 1;
		u64 wordsArrayBit = wordsLength * wordsCount;
		u64 wordsArrayLen = wordsArrayBit / B64 + (wordsArrayBit % B64 ? 1 : 0);
		words = new u64[wordsArrayLen]; memset(words, 0, sizeof(u64) * wordsArrayLen); indexSize += sizeof(u64) * wordsArrayLen;

	}
	bitIndex(const bitIndex &b);
	bitIndex & operator=(const bitIndex &b); //deep copy
	~bitIndex()
	{
		if (posRank) delete posRank;
		if (flagRank) delete flagRank;
		if (fsuccinct) delete[] fsuccinct;
		if (blockOffset) delete[] blockOffset;
		if (words) delete[] words;
	}

public:
	void encode(vector<BTuple> &vb, u64 &offPos, u64 &offHybrid, u64 &offSample, u64 &offGamma, u64 &offWords);
	void encodeGama(vector<int> &value, u64 &offHybird, u64 &offSample, u64 &offGamma);
	void encodeFix(vector<int> &value, u64 &offHybrid, u64 &offSample, u64 &offWords);

public:

	inline u32 decodeValue(const u64 &pos)
	{
		register u64 offsetBlock = pos / blockSize;
		register u64 off = pos % blockSize;
		register u64 offsetFSuccinct = bitOperation::getbit(blockOffset, offsetBlock * sampleFSLength, sampleFSLength);

		if (flagRank->checkOne(offsetBlock))
		{
			return this->decodeGama(offsetFSuccinct, off);
		}
		else                               
		{
			register u32 offsetWord = flagRank->rankValueZero(offsetBlock) - 1;
			u64 fixWords = bitOperation::getbit(words, offsetWord * wordsLength, wordsLength);
			u64 offsetFSFixed = offsetFSuccinct + off * fixWords;
			return bitOperation::getbit(this->fsuccinct, offsetFSFixed, fixWords);
		}
	}
	inline u32 decodeGama(u64 &pos, const u32 &offset)
	{
		register u32 value = 0;
		register u32 countR4 = 0;
		while (1)
		{
			value = bitOperation::getbit(this->fsuccinct, pos, 16);
			countR4 += this->bm->getR4(value);
			if (countR4 <= offset)
			{
				pos += this->bm->getR3(value);
				continue;
			}
			else
			{
				int index = offset - (countR4 - this->bm->getR4(value));
				return this->bm->uncode(index, value); 
			}
		}
		return -1; 
	}
	inline void decodeLeafList(const register u64 &begin, const register u64 &end, BTuple bt[], u32 &count)
	{
		count = 0;
		for (u64 j = begin; j <= end; j++)
		{
			bool f = this->posRank->checkOne(j);
			if (f)
			{
				int pos = this->posRank->rankValueOne(j);
				int value = this->decodeValue(pos - 1);
				bt[count].column = j - begin;
				bt[count].value = value;
				count++;
			}
		}
	}
	inline BList *decodeLeafList(const u64 &begin, const u64 &end)
	{
		BList *bl = new BList();
		for (u64 j = begin; j <= end; j++)
		{
			bool f = this->posRank->checkOne(j);
			if (f)
			{
				int pos = this->posRank->rankValueOne(j);
				int value = this->decodeValue(pos - 1); 
				bl->push_back(BTuple(j - begin, value));
			}
		}
		return bl;
	}
	inline u32 unionListSumInternal(BList *bl, const u64 &begin, const u64 &end)
	{
		int sum = 0;
		for (int j = 0; j < bl->length; j++)
		{
			u64 offset = begin + bl->bt[j].column; 
			if (offset > end)
				continue;
			bool flag = this->posRank->checkOne(offset);
			if (flag)
			{
				int pos = this->posRank->rankValueOne(offset);
				int value = this->decodeValue(pos - 1);
				sum += min(value, bl->bt[j].value);
			}
		}
		return sum;
	}

public:
	void test();
	u32 bitIndexSize()
	{
		return this->indexSize;
	}

};
#endif
