#include "stdafx.h"
#include "edgeHash.h"

edgeHash* edgeHash::instance = NULL;
edgeHash* edgeHash::getInstance()
{
	if (instance == NULL)
		instance = new edgeHash();
	return instance;
}

map<int, int> edgeHash::queryVector(map<string, int> &fs)
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

void edgeHash::insert(const string &str, int &offset)
{
	int pos;
	int hashValue;
	int value = find(str, pos, hashValue);
	offset = hashValue;
	if (value == -1) //not occuring in the hash table
	{
		hashTable[hashValue] = str;
		offsetID.insert(pair<int, int>(hashValue, edgeIndex++));
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