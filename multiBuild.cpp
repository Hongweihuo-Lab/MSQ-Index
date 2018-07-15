#include "multiQSTree.h"

int main(int argc, char *argv[])
{
	if(argc < 5){cout << "parameter error: db  total  prefix length" << endl; return 0;}

	string db = argv[1];
	int total = atoi(argv[2]);
	
	string prefix = argv[3];
	int l = atoi(argv[4]);
	if(argc == 6) 
		BBS = VBS = EBS = atoi(argv[5]);	

	struct timeval start, end; 
	float timeuse; 
	gettimeofday(&start,NULL); 
	
	multiQSTree *mst = new multiQSTree(prefix, l);	
	mst->multiBuildSuccTree(db, total);
	cout << "build bt done" << endl;
	
	gettimeofday(&end,NULL); 
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec; 
	timeuse = timeuse * 1.0 / 1000000; 
	double size = mst->multiSuccTreeSize();
	cout << "building time and index size: " << timeuse << "s, " << size << "MB" << endl;	
		
	if(mst) delete mst;
	return 0;
}
