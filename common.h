#ifndef _COMMON_H
#define _COMMON_H
#include "stdafx.h"
#include "BList.h"
class common
{
public:

	static string inToString(string prefix, int v, int e)
	{
		stringstream ss;
		ss << v << e;
		return prefix + "/" + ss.str();
	}
	static string inToString(int n)
	{
		stringstream ss;
		ss << n;
		return ss.str();
	}
	static void degreeEditDistance(int ds1[], int size1, int ds2[], int size2, int &EI, int &ED)
	{
		EI = ED = 0;
		int sz = min(size1, size2);
		for (int i = 0; i < sz; i++)
		{
			if (ds1[i] < ds2[i])
				EI += ds2[i] - ds1[i];
			else
				ED += ds1[i] - ds2[i];
		}
		if (sz == size1)
		{
			for (int i = sz; i < size2; i++)
				EI += ds2[i];
		}
		else
		{
			for (int i = sz; i < size1; i++)
				ED += ds1[i];
		}
		if (EI % 2 == 0) 
		{
			EI = EI / 2;
			if(ED % 2 == 0)
				ED = ED /2;
			else
				ED = ED/2+1; 
		}
		else
		{
			EI = EI/2 + 1;
			if(ED % 2 == 0)
				ED = ED /2;
			else
				ED = ED/2 + 1;
		}
	}
	static int degreeEditDistance(int ds1[], int size1, int ds2[], int size2)
	{
		int sz = min(size1, size2);
		int s1 = 0, s2 = 0;
		for (int i = 0; i < sz; i++)
		{
			if (ds1[i] < ds2[i])
				s1 += ds2[i] - ds1[i];
			else
				s2 += ds1[i] - ds2[i];
		}
		if (sz == size1)
		{
			for (int i = sz; i < size2; i++)
				s1 += ds2[i];
		}
		else
		{
			for (int i = sz; i < size1; i++)
				s2 += ds1[i];
		}
		if(s1 % 2 == 0)
		{
			if(s2 % 2 == 0)
				return s1 / 2 + s2 / 2;
			else
				return s1 / 2 + s2 / 2 + 1;
		}
		else
		{
			if (s1 % 2 == 0)
				return s1 / 2 + s2 / 2 + 1;
			else
				return s1 / 2 + s2 / 2 + 2;
		}
	} 
	static int degreeEditDistance(vector<int> ds1, vector<int> ds2)
	{
		int sz = max(ds1.size(), ds2.size());
		for (int i = ds1.size(); i < sz; i++)
			ds1.push_back(0);
		for (int i = ds2.size(); i < sz; i++)
			ds2.push_back(0);
		int s1 = 0, s2 = 0;
		int size = ds1.size();
		sort(ds1.begin(), ds1.end(), greater<int>());
		sort(ds2.begin(), ds2.end(), greater<int>());
		for (int i = 0; i < size; i++)
		{  
			if (ds1[i] < ds2[i])
				s1 += ds2[i] - ds1[i]; //add edge
			else 
				s2 += ds1[i] - ds2[i]; //delete edge
		}
		
		if(s1 % 2 == 0)
		{
			if(s2 % 2 == 0)
				return s1 / 2 + s2 / 2;
			else
				return s1 / 2 + s2 / 2 + 1;
		}
		else
		{
			if (s1 % 2 == 0)
				return s1 / 2 + s2/ 2 + 1;
			else
				return s1 / 2 + s2/ 2 + 2;
		}
	} 
	static int unionStringSet(map<string, int> &s1, map<string, int> &s2)
	{
		map<string, int> ::iterator iter, it;
		int sum = 0;
		for (iter = s1.begin(); iter != s1.end(); ++iter)
		{
			it = s2.find(iter->first);
			if (it != s2.end())
				sum += min(iter->second, it->second);
		}
		return sum;
	}

	//unionSet<>(Set A * B) for vertex and edge label
	static int unionSet(map<int, int> &lv1, map<int, int> &lv2)
	{
		map<int, int>::iterator iter, it;
		int sum = 0;
		for (iter = lv1.begin(); iter != lv1.end(); ++iter)
		{
			it = lv2.find(iter->first);
			if (it != lv2.end())
				sum += iter->second < it->second ? iter->second : it->second;
		}
		return sum;
	}
	//the edit distance between two star structure
	//there may occur the problem that: s1[i] occupied two position
	//we must assuming that: the vector has been ordered before into the process
	//Warning that: v1 and v2 must sort according to its value
	static int unionElem(vector<int> &v1, vector<int> &v2)
	{
		int i = 0, j = 0;
		int total = 0;
		while (i < v1.size() && j < v2.size())
		{
			if (v1[i] == v2[j])
			{
				total++;
				i++;
				j++;
			}
			else if (v1[i] < v2[j]) i++;
			else
				j++;

 		}
		return total;
	}
	static int unionResult(BList *ba, BList *bb)
	{
		int sum = 0;
		int i = 0, j = 0;
		while (i < ba->length && j < bb->length)
		{
			if (ba->bt[i].column == bb->bt[j].column)
			{
				sum += min(ba->bt[i].value, bb->bt[j].value);
				i++;
				j++;
			}
			else if (ba->bt[i].column < bb->bt[j].column)
				i++;
			else
				j++;
		}
		return sum;
	}
	
	static bool next_data(int r, int n, vector<int> &temp)
	{
		int i;
		int index = 0;
		for (i = r; i > 0; i--)
		{
			if (temp[i - 1] < n - 1 - r + i) 
			{
				index = i;
				break;
			}
		}
		if (index == 0) return false;
		index--;
		temp[index] = temp[index] + 1;
		for (i = index + 1; i < r; i++)
			temp[i] = temp[i - 1] + 1;
		return true;
	}

	static int deleteEdge(const vector<int> &dg1, const vector<int> &dg2)
	{
		
		int s1 = 0, s2 = 0;
		for (int i = 0; i < dg1.size(); i++)
			s1 += dg1[i];
		for (int i = 0; i < dg2.size(); i++)
			s2 += dg2[i];
		return (s1 - s2) / 2;
	}
//search 
	static bool inVector(vector<int> &ga, const int &value)
	{
		vector<int>::iterator iter = std::find(ga.begin(), ga.end(), value);
		if (iter == ga.end()) return false;
		else return true;
	}

	static void generateZuhe(int n, int m, vector<vector<int> > &results)
	{
		results.clear(); 
		int a[1024], b[1024]; memset(a, 0, sizeof(a)); memset(b, 0, sizeof(b));		
		for (int i = 0; i < m; i++)
			a[i] = 1;
		for (int i = 0; i < n; i++)
			b[i] = i + 1;
		vector<int> tmp;
		for (int i = 0; i < m; i++)
		{
			tmp.push_back(b[i] - 1);
		}
		results.push_back(tmp);
		while (prev_permutation(a, a + n))
		{
			tmp.clear();
			for (int i = 0; i < n; i++)
			{
				if (a[i]) 
					tmp.push_back(b[i] - 1);
			}
			results.push_back(tmp);
		}
		
	}

};

#endif
