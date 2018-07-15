#ifndef _QUERYRECT_H
#define _QUERYRECT_H

#include "branch.h"
#include "globalHash.h"
#include "graph.h"

class queryRect
{
public:
	branch *rect;
	int degree[1024];

public:
	queryRect(){ rect = 0; }
	~queryRect(){	if (rect) delete rect; rect = 0;}
	queryRect(graph &g)
	{
		map<string, twoTuple> fs = g.branchGramSetDegree();
		map<int, int> mv = bh->queryVector(fs);
		map<string, int> vl = g.vertexLabel();
		map<int, int> mvl = vh->queryVector(vl);
		map<string, int> el = g.edgeLabel();
		map<int, int> mel = eh->queryVector(el);
		int count = 0; 
		bh->queryDegree(mv, this->degree, count);
		if(count != g.v)
		{
			g.printf_graph();			
			for(int i = 0; i < count;i++)
				cout << this->degree[i] << " " ;
			cout << "-------" << g.v << "-----" << endl;
			for(int i = 0; i < g.v;i++)
			{
				int d = 0;
				for(int j = 0; j < g.v;j++)
				{
					if(g.Edge[i][j] != -1) 
						d++;
				}
				cout << d << " " ;
			}
			cout << endl; 

			map<string, twoTuple> ::iterator siter; 
			for(siter = fs.begin(); siter != fs.end(); siter++)
			{
				cout << siter->first << " " << siter->second.row<< " " << siter->second.value  <<  endl;
			}

			map<int, int> ::iterator iter;
			for(iter = mv.begin(); iter != mv.end(); iter++)
			{
				cout << iter->first << " " << iter->second << endl;				 
			}

		}
		assert(count == g.v);
		sort(degree, degree + g.v, greater<int>()); 		
		this->rect = new branch(mv, mvl, mel, g.v, g.e);	
		
	}	
};


#endif
