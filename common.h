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
	static int degreeEditDistance(uchar ds1[], u32 &size1, uchar ds2[], u32 &size2)
	{
		int sz = min(size1, size2);
		int s1 = 0, s2 = 0;
		sort(ds1, ds1 + size1, greater<int>());
		sort(ds2, ds2 + size2, greater<int>());
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

};

#endif
