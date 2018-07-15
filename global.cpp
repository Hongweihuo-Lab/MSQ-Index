#include "global.h"

extern int BBS = 16;
extern int EBS = 16;
extern int VBS = 16;

extern u64 g_vertex = 0;
extern u64 g_edge = 0;
extern u64 g_vertex_begin = 0;
extern u64 g_vertex_end = 0;
extern u64 g_edge_beign = 0;
extern u64 g_edge_end = 0;
extern u64 g_branch_begin = 0;
extern u64 g_branch_end = 0;
extern BTuple g_branch_tuple[1024] = {};
extern int g_branch_count = 0; 
extern int g_degree[1024] = { 0 };
extern int g_count = 0;
extern int q_degree[1024] = {}; 
extern int q_count = 0;

extern Succinct SD = Succinct(0, 0, 0, 0);
extern Succinct SV = Succinct(0, 0, 0, 0);
extern Succinct SE = Succinct(0, 0, 0, 0);

extern u64 Ma = 0;
extern u64 Mb = 0; 
extern u64 Mc= 0;

extern map<int, int> MD = map<int, int> ();
extern map<int, int> ML = map<int, int> (); 