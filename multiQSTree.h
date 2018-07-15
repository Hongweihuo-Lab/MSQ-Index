#ifndef _MULTIQSTREE_H
#define _MULTIQSTREE_H

#include "succBitTree.h"
static int filterQueryRegion = 0;

class multiQSTree
{
public:
	map<string, branchTree *> mbt;
	map<string, succBitTree *> mst;

	int maxv;  // the dividision poionts
	int maxe;
	int l; // the area length
	string prefix; //the data area path

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
		else
		{
			string bhf = prefix + "/bhf";
			string vhf = prefix + "/vhf";
			string ehf = prefix + "/ehf";
			remove(bhf.c_str());
			remove(vhf.c_str());
			remove(ehf.c_str());
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
		if (bh)
		{
			bh->sequenceHash(prefix + "/bhf");
			bh = NULL;
		}
		if (vh)
		{
			vh->sequenceHash(prefix + "/vhf");
			vh = NULL;
		}
		if (eh)
		{
			eh->sequenceHash(prefix + "/ehf");
			eh = NULL;
		}
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
	//(x, y) ---->(y + x, y - x)
	inline void transformXYtoUV(int &x, int &y)
	{
		int temp = x;
		x = y + x;
		y = y - temp;
	}
	void staticGraphDataBase(const char *input, int total);
	void position(int tv, int te, int &x, int &y); //ensure the boundary of tv and te
	void writeIntoDisk(string dinfo, string bhf, string vhf, string ehf);
	void writeTreeIntoDisk(); //default into:
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
				cout << "mkdir error in" << prefix << endl;
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
public:
	inline vector<int> multiSuccTreeQuery(graph &q, int d, int k)
	{
		vector<string> queryArea;
		queryArea = unionArea(q.v, q.e, d);
		map<string, succBitTree *> ::iterator iter;
		vector<int> rs;

		vector<vector<queryRect *> > q_subgraphs_sets;
		queryRect *qg = new queryRect(q);
		vector<queryRect *> q_subset_0(1, qg);
		q_subgraphs_sets.push_back(q_subset_0);
		for (int i = 1; i <= k; i++)
		{
			vector<vector<int > > combine;
			common::generateZuhe(q.v, i, combine); //C(q.v, i)
			vector<queryRect *> q_subset_i;
			for (int j = 0; j < combine.size(); j++)
			{
				graph sub;
				q.generateInducedGraph(combine[i], sub);
				q_subset_i.push_back(new queryRect(sub));
			}
			q_subgraphs_sets.push_back(q_subset_i);
		}
		for (int i = 0; i < queryArea.size(); i++)
		{
			iter = mst.find(queryArea[i]);
			if (iter != mst.end() && iter->second)
			{
				filterQueryRegion  += iter->second->totalGraphs;
				vector<int> tmp = iter->second->boostedQuery(q_subgraphs_sets, d);
				for (int j = 0; j < tmp.size(); j++)
					rs.push_back(tmp[j]);
			}
		}
		//free the space
		for (int j = 0; j <= k; j++)
		{
			int size = q_subgraphs_sets[j].size();
			for (int l = 0; l < size; l++)
			{
				if (q_subgraphs_sets[j][l])
					delete q_subgraphs_sets[j][l];
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
