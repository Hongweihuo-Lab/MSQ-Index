#include "stdafx.h"
#include "multiQSTree.h"

void multiQSTree::staticGraphDataBase(const char *input, int total)
{
	FILE *fr = fopen(input, "r"); assert(fr);
	int gid, v, e;
	int vl;
	int f, t, l;
	map<int, int> vc;
	map<int, int> ec;
	map<int, int>::iterator iter;
	int  max;
	int count = 0;

	while (!feof(fr))
	{
		fscanf(fr, "%d\n", &gid);
		fscanf(fr, "%d %d\n", &v, &e);
		iter = vc.find(v);
		if (iter == vc.end())
		{
			vc.insert(pair<int, int>(v, 1));
		}
		else
			iter->second++;
		iter = ec.find(v); 
		
		if (iter == ec.end()) { ec.insert(pair<int, int>(e, 1)); }
		else iter->second++;
		for (int i = 0; i < v; i++)
		{
			fscanf(fr, "%d\n", &vl);
		}
		for (int i = 0; i < e; i++)
		{
			fscanf(fr, "%d %d %d\n", &f, &t, &l);
		}
		count++;
		if (count % 500000 == 0)
			cout << count << endl;
		if (count >= total)
			break;
	}
	if (fr) fclose(fr);
	max = 0;
	for (iter = vc.begin(); iter != vc.end(); ++iter)
	{
		if (max < iter->second)
		{
			max = iter->second;
			maxv = iter->first;
		}
	}

	max = 0;
	for (iter = ec.begin(); iter != ec.end(); ++iter)
	{
		if (max < iter->second)
		{
			max = iter->second;
			maxe = iter->first;
		}
	}
	cout << "the maximal occ:(" << maxv << "," << maxe << ")" << endl;
}

void multiQSTree::position(int tv, int te, int &x, int &y)
{
	const int pointx = maxe + maxv - l; 
	const int pointy = maxe - maxv -l;

	int tx = tv + te;
	int ty = te - tv;
	if (tx >= pointx)
	{
		int lx = tx - pointx;
		x = lx / (2 * l); 
	}
	if (tx < pointx)
	{
		int lx = pointx - tx;
		x = -(lx / (2 * l)) - 1;
	}
	if (ty >= pointy)
	{
		int ly = ty - pointy;
		y = ly / (2 * l) ;
	}
	if (ty < pointy)
	{
		int ly = pointy - ty;
		y = -(ly / (2 * l)) - 1;
	}
}
void multiQSTree::writeIntoDisk(string dinfo, string bhf, string vhf, string ehf)
{	
	ofstream fout(dinfo);
	fout << maxv << "\t" << maxe << "\t" << l << endl;

	fout.close();
	bh->sequenceHash(bhf);
	vh->sequenceHash(vhf);
	eh->sequenceHash(ehf);  
}
void multiQSTree::writeTreeIntoDisk()
{
	map<string, branchTree *> ::iterator iter;
	for (iter = mbt.begin(); iter != mbt.end(); ++iter)
	{
		string path = iter->first + "/bt.idx";
		string offset = iter->first + "/off.idx";
		sTuple bt, vt, et;
		u64 idx;
		u32 maxve; 
		iter->second->treeBitEncodingLen(bt, vt, et, maxve, idx);
		
		SD.I_X += bt.bitLen; SD.V_X += bt.codeNumber;
		SV.I_X += vt.bitLen; SV.V_X += vt.codeNumber; 
		SE.I_X += et.bitLen; SE.V_X += et.codeNumber;
		
		FILE *fw = NULL;
		fw = fopen(offset.c_str(), "wb+"); assert(fw);
		fwrite(&bt, sizeof(sTuple), 1, fw);
		fwrite(&vt, sizeof(sTuple), 1, fw);
		fwrite(&et, sizeof(sTuple), 1, fw);
		fwrite(&idx, sizeof(u64), 1, fw);
		fwrite(&maxve, sizeof(u32), 1, fw);
		fwrite(&iter->second->totalGraphs, sizeof(u32), 1, fw);		

		iter->second->writeIntoDisk(path); 
		if (iter->second) delete iter->second;
		iter->second = NULL; 
		fclose(fw); fw = NULL;
	}
}
void multiQSTree::readTreeFromDisk()
{
	map<string, branchTree *> ::iterator iter;
	for (iter = mbt.begin(); iter != mbt.end(); ++iter)
	{
		succBitTree *sbt = new succBitTree(iter->first + "/off.idx");
		sbt->buildSuccBitTree(iter->first + "/bt.idx"); // 
		mst.insert(pair<string, succBitTree* >(iter->first, sbt)); // ok, done
	}
}

void multiQSTree::multiBuildSuccTree(string db, int total)
{
	this->staticGraphDataBase(db.c_str(), total);
	cout << "static done" << endl;

	FILE *fr = fopen(db.c_str(), "r+"); ASSERT(fr);
	int gid, v, e;
	int f, t, l;
	int count = 0;
	map<string, branchTree*>::iterator iter;

	while (!feof(fr))
	{
		fscanf(fr, "%d\n", &gid);
		fscanf(fr, "%d %d\n", &v, &e);
		vector<int> vc(v, 0);
		for (int i = 0; i < v; i++)
		{
			fscanf(fr, "%d\n", &vc[i]);
		}
		
		vector<int> adj(v, -1);
		vector<vector<int > > adjMatrix(v, adj);

		for (int i = 0; i < e; i++)
		{
			fscanf(fr, "%d %d %d\n", &f, &t, &l);
			adjMatrix[f][t] = l;
			adjMatrix[t][f] = l;
		}
		graph g(gid, v, vc, e, adjMatrix);

		string areaID = this->enchodingArea(v, e); //areaID
		iter = mbt.find(areaID);
		if (iter != mbt.end())
			iter->second->Insert(g); //insert the graph g into the disk
		else
		{
			branchTree *bt = new branchTree();
			bt->Insert(g);
			mbt.insert(pair<string, branchTree *>(areaID, bt));			
		}
		count++;
		if (count % 200000 == 0)
			cout << count << endl;
		if (count >= total)
			break;
	}
	if (fr) fclose(fr);

	this->writeTreeIntoDisk(); //
	this->readTreeFromDisk();  //
	cout << "begin build the succinctTree" << endl;
}

double multiQSTree::multiSuccTreeSize()
{
	double sum = 0.0;
	map<string, succBitTree *> ::iterator iter;
	for (iter = mst.begin(); iter != mst.end(); ++iter)
		sum += iter->second->succinctBitTreeSize();
	return sum;
}

void multiQSTree::multiBuildSuccTree(string prefix)
{
	assert(this->prefix == prefix); 
	string dfo = prefix + "/dfo";

	ifstream fin(dfo);
	fin >> this->maxv >> this->maxe >> this->l;
	vector<string> QPath; int size;
	fin >> size;
	for (int i = 0; i < size; i++)
	{
		string str;
		fin >> str;
		QPath.push_back(str);
	}

	assert(size == QPath.size());
	for (int i = 0; i < size; i++)
	{
		string str = QPath[i];
		succBitTree *sbt = new succBitTree(str + "/off.idx");
		sbt->buildSuccBitTree(str + "/bt.idx"); //
		mst.insert(pair<string, succBitTree* >(str, sbt)); // ok, done
	}
}
