#pragma once 
#ifndef _BITMAP_H
#define _BITMAP_H
#include "stdafx.h"
class bitmap
{
private:
	u32 *bmap;
	static bitmap* instance;
public:
	static bitmap* getInstance();	
	u32 getR1(u32 &i);
	u32 getR2(u32 &i);
	u32 getR3(u32 &i);
	u32 getR4(u32 &i);
	u32 *getGamaArr(u32 &i);
	int uncode(int i, u32 value);
	u32* getBitmap();
	~bitmap();
private:
	bitmap();
};
#endif