#include "stdafx.h"
#include "branchHash.h"

branchHash* branchHash::instance = NULL;
branchHash* branchHash::getInstance()
{
	if (instance == NULL)
		instance = new branchHash();
	return instance;
}

map<int, int> branchHash::queryVector(map<string, int> &fs)
{
	map<string, int>::iterator iter;
	map<int, int> qv;
	int offset;
	for (iter = fs.begin(); iter != fs.end(); iter++)
	{
		string str = iter->first;
		int v = iter->second;
		insert(str, offset);
		int branchID = getBranchID(offset);
		qv.insert(pair<int, int>(branchID, v));
	}
	return qv;
}

void branchHash::insert(const string &str, int &offset)
{
	int pos;
	int hashValue;
	int value = find(str, pos, hashValue);
	offset = hashValue;
	if (value == -1) 
	{
		hashTable[hashValue] = str;
		offsetID[hashValue] = hashIndex;
		offsetInverse[hashIndex] = hashValue;
		hashIndex++;
		return;
	}
	else
	{
		string s = hashTable[hashValue];
		if (s == str)
			return;
		else
		{
			cout << "error in insert string" << endl;
			return;
		}
	}
}