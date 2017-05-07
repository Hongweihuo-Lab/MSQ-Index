#pragma once
#ifndef _BRANCHHASH_H
#define _BRANCHHASH_H

#include "stdafx.h"
#include "BList.h"
#define MAX_COLUMN 10001

class branchHash
{
private:
	string hashTable[MAX_COLUMN]; //hashTable[hash(str)] = str
	int degree[MAX_COLUMN]; // degree[hash(str)] = degree(str)
	int offsetID[MAX_COLUMN]; // offsetID[hash(str)] = ID(str)
	int offsetInverse[MAX_COLUMN]; //offsetInverse[offsetID] = offset;
	int length[MAX_COLUMN]; //record the length of str[i]
	int hashIndex;
	string SENTRY;

public:
	static branchHash * instance;
	static branchHash * getInstance();
private:
	branchHash()
	{
		hashIndex = 0;
		SENTRY.resize(16, '$');
		for (int i = 0; i < MAX_COLUMN; i++)
		{
			hashTable[i] = SENTRY;
			degree[i] = -1;
			offsetID[i] = -1;
			offsetInverse[i] = -1;
			length[i] = 16;
		}
	}
public:
	void queryDegree(BTuple bt[], u32 &countTuple, uchar degree1[], u32 &count)
	{
		count = 0;
		for (int i = 0; i < countTuple; i++)
		{
			int value = offsetInverse[bt[i].column]; //offset
			int d = degree[value];
			for (int j = 0; j < bt[i].value; j++)
				degree1[count++] = d;
		}
	}
	void queryDegree(BList *bl, uchar degree1[], u32 &count)
	{
		count = 0;
		for (int i = 0; i < bl->length; i++)
		{
			int value = offsetInverse[bl->bt[i].column]; //offset
			int d = degree[value];
			for (int j = 0; j < bl->bt[i].value; j++)
				degree1[count++] = d;
		}
	}
	void queryDegree(map<int, int> &qv, uchar degree1[], u32 &count)
	{
		count = 0;
		map<int, int> ::iterator iter;
		for (iter = qv.begin(); iter != qv.end(); iter++)
		{
			int value = offsetInverse[iter->first]; //offset
			int d = degree[value];
			for (int i = 0; i < iter->second; i++)
				degree1[count++] = d;
		}
	}
	vector<int> queryDegree(BList *bl)
	{
		vector<int> vs;
		for (int i = 0; i < bl->length;i++)
		{
			int value = offsetInverse[bl->bt[i].column]; //offset
			if (value == -1)
			{
				cout << "error in the offsetID :" << endl;
				exit(0);
			}
			int d = degree[value];
			for (int j = 0; j < bl->bt[i].value; j++)
				vs.push_back(d);
		}
		return vs;
	}

	vector<int> queryDegree(map<int, int> &qv)
	{
		vector<int> vs;
		map<int, int> ::iterator iter;
		for (iter = qv.begin(); iter != qv.end(); iter++)
		{
			int value = offsetInverse[iter->first]; //offset
			if (value = -1)
			{
				cout << "error in the offsetID :" << endl;
				exit(0);
			}
			int d = degree[value];
			for (int i = 0; i < iter->second; i++)
				vs.push_back(d);
		}
		return vs;
	}
	map<int, int> queryVector(map<string, int> &fs);
	map<int, int> queryVector(map<string, twoTuple> &fs)
	{
		map<string, twoTuple>::iterator iter;
		map<int, int> qv;
		int offset;
		for (iter = fs.begin(); iter != fs.end(); iter++)
		{
			string str = iter->first;
			int v = iter->second.row;
			insert(str, offset);
			this->degree[offset] = iter->second.value;
			int branchID = getBranchID(offset);
			qv.insert(pair<int, int>(branchID, v));
		}
		return qv;
	}
	void sequenceHash(string hashFile)
	{
		int size = MAX_COLUMN;
		for (int i = 0; i < size; i++)
			length[i] = hashTable[i].length();
		
		FILE *fw = fopen(hashFile.c_str(), "wb+"); assert(fw);
		fwrite(&size, sizeof(int), 1, fw);
		for (int i = 0; i < size; i++)
			fwrite(&offsetID[i], sizeof(int), 1, fw);
		for (int i = 0; i < size; i++)
			fwrite(&degree[i], sizeof(int), 1, fw);
		for (int i = 0; i < size; i++)
			fwrite(&length[i], sizeof(int), 1, fw);
		for (int i = 0; i < size; i++)
			fwrite(hashTable[i].c_str(), sizeof(char), length[i], fw);
		if (fw) fclose(fw); fw = NULL;
	}

	 void readHashTable(string hashFile)
	 {
		 int size, value;
		 char ch[32];

		 FILE* fr = fopen(hashFile.c_str(), "rb+"); assert(fr);
		 fread(&size, sizeof(int), 1, fr);

		 for (int i = 0; i < size; i++)
		 {
			 fread(&value, sizeof(int), 1, fr);
			 offsetID[i] = value;
		 }
		 for (int i = 0; i < size; i++)
		 {
			 fread(&value, sizeof(int), 1, fr);
			 degree[i] = value;
		 }
		 for (int i = 0; i < size; i++)
		 {
			 fread(&value, sizeof(int), 1, fr);
			 length[i] = value;
		 }
		 for (int i = 0; i < size; i++)
		 {
			 fread(ch, sizeof(char), length[i], fr);
			 ch[length[i]] = '\0';
			 hashTable[i] = ch;
		 }
		 for (int i = 0; i < size; i++)
		 {
			 if (offsetID[i] != -1)
			 {
				 offsetInverse[offsetID[i]] = i;
			 }
		 }
		 if (fr) fclose(fr);
	 }
	 int HashSize()
	 {
		 int sum = MAX_COLUMN * 16;
		 sum += sizeof(int) * 3 * MAX_COLUMN;
		 return sum;
	 }

private:

	 int DJBHASH(const char *str)
	 {
		unsigned long hash = 5381;
		int c;
		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		return hash % MAX_COLUMN;
	 } 
	 void strCollapse(string &str, int value)
	 {
		stringstream ss;
		ss << value;
		str += ss.str();
	 }

	 int getBranchID(const int &offset)
	 {
		assert(offset < MAX_COLUMN);
		int iter = offsetID[offset];
		if (iter != -1)
			return iter;
		else
		{
			cout << "the mapping relationship error" << endl;
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