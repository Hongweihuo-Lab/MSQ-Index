#include "test.h"

int main()
{
	test * t = new test();
	//t->testBitIndex();
	//t->testRankIndex();
	//t->testQSuccinctTree(AIDS, AIDS_Query, 42687);
	t->testMultiQSTree(AIDS, AIDS_Query, 42687);
	system("pause");
	return 0;
}