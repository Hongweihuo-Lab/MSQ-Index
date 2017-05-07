#ifndef _MULTIQSTREE_H
#define _MULTIQSTREE_H
#include "succBitTree.h"

class multiQSTree
{
public:
	map<string, branchTree *> mbt;
	map<string, succBitTree *> mst;
	
	vertexHash *vh;
	edgeHash *eh;
	branchHash *bh;
	int maxv;  
	int maxe; 
	int l; 
	string prefix; 
	
public:
	multiQSTree(string prefix, int l)
	{
		bh = branchHash::getInstance();
		vh = vertexHash::getInstance();
		eh = edgeHash::getInstance();
		maxv = maxe = 0;
		this->l = l;
		this->prefix = prefix;
		mbt.clear();
		mst.clear();
		if (access(prefix.c_str(), F_OK | R_OK) == -1)
		{
			if (mkdir(prefix.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
			{
				cout << "mkdir error in" << prefix << endl;
			}
		}
	}		
	~multiQSTree()
	{
		for (map<string, branchTree *> ::iterator iter = mbt.begin(); iter != mbt.end(); ++iter) //delete the space
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = NULL;
			}
		}
		ofstream fout(prefix + "/dfo");
		fout << maxv << "\t" << maxe << "\t" << l << endl;
		fout << mst.size() << endl;
		for (map<string, succBitTree *> ::iterator iter = mst.begin(); iter != mst.end(); ++iter)
		{
			fout << iter->first << endl;
			if (iter->second)
			{
				delete iter->second;
				iter->second = NULL;
			}
		}
		fout.close();
		if (bh) bh->sequenceHash(prefix + "/bhf"); bh = NULL;
		if (vh) vh->sequenceHash(prefix + "/vhf"); vh = NULL;
		if (eh) eh->sequenceHash(prefix + "/ehf"); eh = NULL;
	}
	multiQSTree(string prefix)
	{
		string bhf = prefix + "/bhf";
		string vhf = prefix + "/vhf";
		string ehf = prefix + "/ehf";
	
		bh = branchHash::getInstance();
		vh = vertexHash::getInstance();
		eh = edgeHash::getInstance();
		bh->readHashTable(bhf);
		vh->readHashTable(vhf);
		eh->readHashTable(ehf);
		this->prefix = prefix;
	}
	
public:	
	inline void transformXYtoUV(int &x, int &y)
	{
		int temp = x;
		x = y + x;
		y = y - temp;
	}
	void staticGraphDataBase(const char *input, int total); 
	void position(int tv, int te, int &x, int &y); //ensure the boundary of tv and te
	void writeIntoDisk(string dinfo, string bhf, string vhf, string ehf); 
	void writeTreeIntoDisk();
	void readTreeFromDisk(); 

public:
	inline string enchodingArea(int &x, int &y)
	{
		int ax, ay;
		position(x, y, ax, ay);
		stringstream ss;
		ss << ax << ay;
		string path = prefix + "/" + ss.str();
		if (access(path.c_str(), F_OK | R_OK) == -1)
		{
			if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)  
			{
				cout << prefix << " " << ss.str() << endl;
				cout << "mkdir error in path" << endl;
			}
		}
		return path;
	}
	inline void unionArea(int v, int e, int &lx, int &rx, int &ly, int &ry, int d)
	{
		int lv = v;
		int rv = v;
		int le = e - d;
		int re = e + d;
		position(lv, le, lx, ly);
		position(rv, re, rx, ry);
	}
	inline vector<string> unionArea(int v, int e, int d)
	{
		vector<string> vs;
		int lx, rx, ly, ry;
		unionArea(v, e, lx, rx, ly, ry, d);

		for (int i = lx; i <= rx; i++)
		{
			for (int j = ly; j <= ry; j++)
			{
				string path = common::inToString(prefix, i, j);
				vs.push_back(path);
			}
		}

		return vs;
	}
	inline vector<int> multiSuccTreeQuery(graph &g, int d)
	{
		vector<string> queryArea;
		queryArea = unionArea(g.v, g.e, d);
		map<string, succBitTree *> ::iterator iter;
		vector<int> rs;
		for (int i = 0; i < queryArea.size(); i++)
		{
			iter = mst.find(queryArea[i]);
			if (iter != mst.end() && iter->second)
			{				 
				vector<int> tmp = iter->second->queryGraph(g, d);
				for (int j = 0; j < tmp.size(); j++)

					rs.push_back(tmp[j]);				
			}

		}
		return rs;
	}

public:
	void multiBuildSuccTree(string db, int total); 
	void multiBuildSuccTree(string prefix); 
	double multiSuccTreeSize();

};

#endif
