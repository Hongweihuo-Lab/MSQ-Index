# MSQ-Index (Multi Succinct Q-gram Tree Index)

## What is it?
MSQ-Index is a succinct index for fast graph similarity search. It can efficiently answer the following query problem: Given a graph database *G = {g1, g2, ..., gn}*, a query graph *h* and an edit distance threshold *t*, find the graphs in *G* are similar to *h*; that is, *Cand = {g: ged(g, h) <= t}*, where *ged(g, h)* is the edit distance between *g* and *h*.
	 
## How to use it?
   MSQ-Index consists of two components, index building and query processing. You should first build the MSQ-Index on the graph database *G*, then perform the query processing. 
### Step I. Install
   1. download (or clone) the source code from https://github.com/Hongweihuo-Lab/MSQ-Index/
   2. Compile the source code. 
### Step II. Build MSQ-Index 
   1. run the shell command: "./multiBuild db db_num pos l", where *db* is the graph database, *db_num* is the number of graphs to be built, *pos* is the index position(e.g., ./index/), and *l* is the subreiogn length. 
### Step III. Query Processing
   1. run the shell command: "./multiQuery pos query_db threshold k", where *pos* is the index position, *query_db* is the query graph sets, *threshold* is the edit distance threshold, and *k* is the boosting 
   parameter. 

## Paper
X. Chen, H. Huo, J. Huan, and J. S. Vitter, et al., MSQ-Index: A succinct index for fast graph similarity search. IEEE Transactions on Knowledge and Data Engineering (TKDE), 33(6):2654-2668, 2021. 

### Feedback
Please report bugs to X. Chen <jackcxy@126.com> if any. Your feedback and test results are welcome.
### LOg
Jan. 02, 2022, Delete redundant header file: rankIndex.h 
