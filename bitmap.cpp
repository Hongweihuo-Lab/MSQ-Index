#include "stdafx.h"
#include "bitOperation.h"
#include "bitmap.h"


bitmap* bitmap::instance = 0;
bitmap* bitmap::getInstance()
{
	if (instance == 0)
	{
		instance = new bitmap();
	}
	return instance;
}
bitmap::bitmap()
{
	this->bmap = new u32[1ULL << 16];
	memset(this->bmap, 0, sizeof(u32) * (1l << 16));
	u16 R1, R2, R3, R4;

	for (int i = (1ULL << 8); i < (1ULL << 16); i++)
	{
		int j = 15; 
		R3 = R4 = 0;
		u16 num0 = 0;
		bool flag = true;

		while (j >= 0)
		{
			u16 bitx = i & (1 << j);
			u32 value = i;
			if (bitx)
			{
				j -= num0;
				if (j >= 0) 
				{
					R4++; 
					int tmp = num0 * 2 + 1;
					value = value << (B32 - tmp - j);
					value = value >> (B32 - tmp);
					if (flag)
					{
						R1 = value & 0xff;
						R2 = tmp;
						flag = false;
					}
				}
				else
				{
					break;
				}
				R3 = 16 - j; 
				num0 = 0;
			}
			else
			{
				num0++;
			}
			j--;
		}
		R3 = R3 & 0xff;
		R4 = R4 & 0xff;
		this->bmap[i] = (R1 << 24) + (R2 << 16) + (R3 << 8) + R4;
	}
}

u32* bitmap::getBitmap()
{
	return this->bmap;
}
u32 bitmap::getR1(u32 &i)
{
	return bmap[i] >> 24;
}
u32 bitmap::getR2(u32 &i)
{
	u32 tmp = this->bmap[i];
	tmp = tmp << 8;
	return tmp >> 24;
}
u32 bitmap::getR3(u32 &i)
{
	u32 tmp = this->bmap[i];
	tmp = tmp << 16;
	return tmp >> 24;
}
u32 bitmap::getR4(u32 &i)
{
	u32 tmp = this->bmap[i];
	tmp = tmp << 24;
	return tmp >> 24;
}
int bitmap::uncode(int i, u32 value)
{
	if (i == 0) return this->getR1(value);
	register int j = 0;
	register int offset;
	while (j < i)
	{
		offset = this->getR2(value);
		value = (value << offset) & 0xffff;
		j++;
	}
	return this->getR1(value);
}
bitmap::~bitmap()
{
	if (bmap) delete[] bmap;
	bmap = NULL;
}
