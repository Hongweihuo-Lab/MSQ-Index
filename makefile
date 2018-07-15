target: bitIndex.o bitmap.o branchHash.o vertexHash.o edgeHash.o global.o globalHash.o branchTree.o succBitTree.o \
	multiQSTree.o multiBuild multiQuery 
 
CC=g++
CFLAGS= -O3 -std=c++0x -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE 

bitIndex.o: bitIndex.cpp
	$(CC) -c  $(CFLAGS) bitIndex.cpp
bitmap.o : bitmap.cpp
	$(CC) -c  $(CFLAGS) bitmap.cpp
branchHash.o: branchHash.cpp
	$(CC) -c  $(CFLAGS) branchHash.cpp
vertexHash.o: vertexHash.cpp
	$(CC) -c  $(CFLAGS) vertexHash.cpp
edgeHash.o: edgeHash.cpp
	$(CC) -c  $(CFLAGS) edgeHash.cpp
global.o: global.cpp
	$(CC) -c  $(CFLAGS) global.cpp
globalHash.o: globalHash.cpp
	$(CC) -c  $(CFLAGS) globalHash.cpp
branchTree.o: globalHash.o branchTree.cpp
	$(CC) -c  $(CFLAGS) globalHash.o branchTree.cpp
succBitTree.o: bitIndex.o branchTree.o  succBitTree.cpp
	$(CC) -c  $(CFLAGS) bitIndex.o branchTree.o succBitTree.cpp
multiQSTree.o: succBitTree.o multiQSTree.cpp
	$(CC) -c  $(CFLAGS)  succBitTree.o multiQSTree.cpp
multiBuild: bitIndex.o bitmap.o global.o branchHash.o vertexHash.o edgeHash.o globalHash.o branchTree.o succBitTree.o multiQSTree.o multiBuild.cpp
	$(CC) $(CFLAGS) bitIndex.o bitmap.o global.o branchHash.o vertexHash.o edgeHash.o globalHash.o branchTree.o succBitTree.o multiQSTree.o multiBuild.cpp -o multiBuild
multiQuery: bitIndex.o global.o branchHash.o vertexHash.o edgeHash.o globalHash.o branchTree.o succBitTree.o multiQSTree.o multiQuery.cpp
	$(CC) $(CFLAGS) bitIndex.o global.o branchHash.o vertexHash.o edgeHash.o globalHash.o branchTree.o bitmap.o succBitTree.o multiQSTree.o  multiQuery.cpp -o multiQuery
clean:
	rm  -rf *~ *.o multiBuild multiQuery
