#pragma once
#ifndef _BRANCHHASH_H
#define _BRANCHHASH_H

#include "stdafx.h"
#include "BList.h"
#define MAXENTRY  100001
class branchHash
{

           //store the degree information:
private:
	string hashTable[MAXENTRY]; //hashTable[hash(str)] = str
	int degree[MAXENTRY]; // degree[hash(str)] = degree(str)
	int offsetID[MAXENTRY]; // offsetID[hash(str)] = ID(str)
	int offsetInverse[MAXENTRY]; //offsetInverse[offsetID] = offset;
	int length[MAXENTRY]; //record the length of str[i]
	string SENTRY;

public:
	 int hashIndex;
	static branchHash * instance;
	static branchHash * getInstance();
private:
	branchHash()
	{		
		hashIndex = 0;
		SENTRY.resize(16, '$');
		for (int i = 0; i < MAXENTRY; i++)
		{
			hashTable[i] = SENTRY;
			degree[i] = -1;
			offsetID[i] = -1;
			offsetInverse[i] = -1;
			length[i] = 16;
		}
	}
public:
	void queryDegree(BTuple bt[],  int &countTuple, int degree1[], int &count)
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
	void queryDegree(BList *bl, int degree1[], int &count)
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
	void queryDegree(map<int, int> &qv, int degree1[], int &count)
	{
		count = 0;
		map<int, int> ::iterator iter;
		for (iter = qv.begin(); iter != qv.end(); iter++)
		{
			int value = offsetInverse[iter->first]; //offset
			int d = degree[value];			
			//cout << value << " " << d << " " << iter->second << endl;
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
			int branchID = getBranchID(offset); //here, occuring colpase
			qv.insert(pair<int, int>(branchID, v));
			//cout <<  str << " " << v << " " << iter->second.value << ":" << offset << " " 
			 //       << branchID << " " <<  qv.size () << endl;
		}
		return qv;
	}
	void sequenceHash(string hashFile)
	{
		int size = MAXENTRY;
		for (int i = 0; i < size; i++)
			length[i] = hashTable[i].length();
		
		FILE *fw = fopen(hashFile.c_str(), "wb+"); assert(fw);		
		fwrite(&hashIndex, sizeof(int), 1, fw);
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
		 char ch[512];

		 FILE* fr = fopen(hashFile.c_str(), "rb+"); assert(fr);
		 
		 fread(&hashIndex, sizeof(int),  1, fr);
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
	 //test for the HashTable Size 
	 int HashSize()
	 {
		 int sum = MAXENTRY * 16;
		 sum += sizeof(int) * 3 * MAXENTRY;
		 return sum;
	 }

private:

	 int DJBHASH(const char *str)
	 {
		unsigned long hash = 5381;
		int c;
		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		return hash % MAXENTRY;
	 } 
	 void strCollapse(string &str, int value)
	 {
		stringstream ss;
		ss << value;
		str += ss.str();
	 }

	 int getBranchID(int &offset)
	 {
		assert(offset < MAXENTRY);
		int iter = offsetID[offset];
		if (iter != -1)
			return iter;
		else
		{
			cout << "the mapping relationship error" << endl;
			return -1;
		}
	 }

	 int  find(string str, int &hashValue)
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