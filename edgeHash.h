#pragma once
#ifndef _EDGEHASH_H
#define _EDGEHASH_H
#include "stdafx.h"

class edgeHash
{
private:
	string hashTable[101]; // store the feature
	int length[101];
	map<int, int> offsetID; //recording sizerelationship: offset->branchID
	int edgeIndex;
	string SENTRY;
public:
	static edgeHash * instance;
	static edgeHash * getInstance();

private:
	edgeHash()
	{
		SENTRY.resize(8, '$');
		edgeIndex = 0;
		for (int i = 0; i < 101; i++)
			hashTable[i] = SENTRY;
	}
public:
	map<int, int> queryVector(map<string, int> &fs);
	void sequenceHash(string hashFile)
	{
		FILE *fw = fopen(hashFile.c_str(), "wb+"); assert(fw);
		int size;
		size = offsetID.size();
		fwrite(&size, sizeof(int), 1, fw);
		for (map<int, int> ::iterator iter = offsetID.begin(); iter != offsetID.end(); iter++)
		{
			int c = iter->first, v = iter->second;
			fwrite(&c, sizeof(int), 1, fw);
			fwrite(&v, sizeof(int), 1, fw);
		}
		size = 101; fwrite(&size, sizeof(int), 1, fw);
		for (int i = 0; i < size; i++)
		{
			length[i] = hashTable[i].length();
		}
		for (int i = 0; i < size; i++)
			fwrite(&length[i], sizeof(int), 1, fw);
		for (int i = 0; i < size; i++)
			fwrite(hashTable[i].c_str(), sizeof(char), length[i], fw);
		if (fw) fclose(fw); fw = NULL;
	}

	void readHashTable(string hashFile)
	{
		FILE* fr = fopen(hashFile.c_str(), "rb+"); assert(fr);
		int size;
		char ch[16];
		fread(&size, sizeof(int), 1, fr);
		for (int i = 0; i < size; i++)
		{
			int c, v;
			fread(&c, sizeof(int), 1, fr);
			fread(&v, sizeof(int), 1, fr);
			offsetID.insert(pair<int, int>(c, v));
		}
		fread(&size, sizeof(int), 1, fr);
		for(int i = 0; i < size;i++)
		{
			fread(&length[i], sizeof(int), 1, fr);
		}
		for (int i = 0; i < size; i++)
		{
			fread(ch, sizeof(char), length[i], fr);
			ch[length[i]] = '\0';
			hashTable[i] = ch;
		}
		if (fr) fclose(fr); fr = NULL;
	}

	//test for the HashTable Size 
	int HashSize()
	{
		int sum = 101 * 16;
		sum += sizeof(int) * 2 * offsetID.size();
		return sum;
	}

private:
	//djb2 hash function
	int DJBHASH(const char *str)
	{
		unsigned long hash = 5381;
		int c;
		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		return hash % 101;
	}
	void strCollapse(string &str, int value)
	{
		stringstream ss;
		ss << value;
		str += ss.str();
	}

	int getBranchID(const int &offset)
	{
		map<int, int>::iterator iter;
		iter = offsetID.find(offset);

		if (iter != offsetID.end())
			return iter->second;
		else
		{
			cout << "sizerelationship error" << endl;
			return -1;
		}
	}

	int  find(string str, int &pos, int &hashValue)
	{
		hashValue = DJBHASH(str.c_str());
		string s = hashTable[hashValue];
		string temp = s;
		while (true)
		{
			if (s == str)
				return hashValue;
			if (s == SENTRY)
				return -1;//find the insertion place
			strCollapse(temp, hashValue);
			hashValue = DJBHASH(temp.c_str());
			s = hashTable[hashValue];
		}
		return -1;
	}
	void insert(const string &str, int &offset);

};

#endif