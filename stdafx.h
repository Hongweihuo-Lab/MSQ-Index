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

#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <time.h>
#define  fseeko _fseeki64
#define stat64 _stati64
#else
#include <sys/time.h>
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
typedef long long __int64;
typedef unsigned char uchar;
typedef unsigned char uchar;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int uint;

using namespace std;