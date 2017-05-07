#include "stdafx.h"
#include "branchTree.h"

void branchTree::buildBranchTree(vector<graph > &vg)
{
	int n = vg.size();
	for (int i = 0; i < n; i++)
		this->Insert(vg[i]);
}
void branchTree::buildBranchTree(string db, int total)
{
	FILE *fr = fopen(db.c_str(), "r+"); assert(fr);
	int v, e;
	int gid;
	int f, t, l;
	int count = 0;

	while (!feof(fr))
	{
		fscanf(fr, "%d\n", &gid);
		fscanf(fr, "%d %d\n", &v, &e);
		vector<int> vc(v, 0);
		for (int i = 0; i < v; i++)
			fscanf(fr, "%d\n", &vc[i]);
		vector<int> adj(v, -1);
		vector<vector<int > > adjMatrix(v, adj);

		for (int i = 0; i < e; i++)
		{
			fscanf(fr, "%d %d %d\n", &f, &t, &l);
			adjMatrix[f][t] = l;
			adjMatrix[t][f] = l;
		}
		graph g(gid, v, vc, e, adjMatrix);
		this->Insert(g);
		count++;
		if (count >= total)
			break;
	}
	if (fr) fclose(fr);
}
void branchTree::writeIntoDisk(const string path)
{
	FILE *fw = fopen(path.c_str(), "wb+"); assert(fw);
	this->sequenceBranchTree(this->root, fw);
	if (fw) fclose(fw); 
}
void branchTree::readFromDisk(const string path)
{
	FILE * fw = fopen(path.c_str(), "rb+"); assert(fw);
	if (this->root) delete this->root;
	readBranchTree(&this->root, fw); 
	if (fw) fclose(fw);
}
double branchTree::branchTreeSize()
{
	u64 sum = 0;
	sum = this->indexSize(this->root);
	double size = sum * 1.0 / (1024.0 * 1024.0);
	cout << "the normal size:" <<size<< "MB" << endl;
	return size;
}
