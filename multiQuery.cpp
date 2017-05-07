#include "multiQSTree.h"

int main(int argc, char *argv[])
{
	if(argc < 4){cout << "parameter error: prefix query threshold " << endl; return 0;}
	
	string prefix = argv[1];
	string query = argv[2];
	int d = atoi(argv[3]);
	
	struct timeval start,end; 
	float timeuse; 	

	multiQSTree *mst = new multiQSTree(prefix);
	mst->multiBuildSuccTree(prefix);

	vector<graph> queryGraph = graph::readGraphMemory(query.c_str(), 50);	
	int graphNumber = queryGraph.size();
	
	gettimeofday(&start, NULL); 
	int sum = 0;
	vector<vector<int > > verifyGraphSet;
	for (int j = 0; j < graphNumber; j++)
	{
		vector<int> result = mst->multiSuccTreeQuery(queryGraph[j], d);
		sum += result.size();
		verifyGraphSet.push_back(result);
		result.clear();		
	}
	gettimeofday(&end, NULL); 
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec; 
	timeuse = timeuse * 1.0 / 1000000; 

	cout << "average Size and query time(s):" << d << " " << sum *1.0 / graphNumber<< " " << timeuse << "s" << endl;
	if(mst) delete mst;	
	return 0;

}
