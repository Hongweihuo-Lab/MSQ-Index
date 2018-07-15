#pragma once
#ifndef _GRAPH_H
#define _GRAPH_H
#include "stdafx.h"
#include "common.h"

class graph
{
public:
	int v;
	vector<int> V;
	vector<vector<int > > Edge;
	int graph_id;
	int e;

public:
	graph(int gid, int num, vector<int> &VINFO, int e, vector<vector<int > > &einfo)
	{
		this->graph_id = gid;
		this->v = num;
		this->V = VINFO;

		this->e = e;
		this->Edge = einfo;
	}
	graph(const graph &g)
	{
		this->graph_id = g.graph_id;
		this->v = g.v;
		this->e = g.e;
		this->V = g.V;
		this->Edge = g.Edge;
	}
	graph(){}
	~graph(){}
	static vector<graph> readGraphMemory(const char *file, int total)
	{
		ifstream fr(file, ios::in);
		vector<graph> vg;
		int gid, v, e;
		int f, t, l;
		int vd;
		int count = 0;

		while (fr >> gid)
		{
			fr >> v >> e;
			vector<int> vc;
			for (int i = 0; i < v; i++)
			{
				fr >> vd;
				vc.push_back(vd); //insert the vertex label into vc
			}
			vector<int> adj(v, -1);
			vector<vector<int> > adjMatrix(v, adj);
			for (int i = 0; i < e; i++)
			{
				fr >> f >> t >> l;
				adjMatrix[f][t] = l;
				adjMatrix[t][f] = l;
			}
			graph g(gid, v, vc, e, adjMatrix);
			vg.push_back(g);
			count++;
			if (count >= total)
				break;
		}
		fr.close();
		return vg;

	}

public:
	bool hasEdge(int from, int to, int &einfo)
	{
		return Edge[from][to] != -1;
	}
	int edgeinfo(int from, int to) 
	{
		assert(from < v && to < v);
		if (Edge[from][to] == -1)
		{
			cout << "no edge in " << from << " and " << to << endl;
		}
		return Edge[from][to];
	}
	int degreeVertex(const int &id)
	{
		int s = 0;
		for (int i = 0; i < v; i++)
		{
			if (Edge[id][i] != -1)
				s++;
		}
		return s;
	}
	int maxDegree()
	{
		int max = 0;
		for (int i = 0; i < v; i++)
		{
			int vd = degreeVertex(i);
			if (max < vd)
				max = vd;
		}
		return max;
	}

	stringInt branchGramVertexDegree(int id)
	{
		assert(id < v);
		int s = V[id];
		char ch[1024]; 
		ch[0] = s + '0';
		stringInt ms;
		int size = 0;

		for (int i = 0; i < v; i++)
		{
			if (Edge[id][i] != -1)
			{
				//ch[size + 1] = Edge[id][i];
				ch[size + 1] = Edge[id][i] + '0';
				size++;
			}
		}

		ch[size + 1] = '\0';
		sort(ch + 1, ch + size + 1);
		ch[size + 1] = size + '0';
		ch[size + 2] = '\0'; 			
		ms.str = ch;
		ms.degree = size;
		return ms;
	}
	map<string, twoTuple> branchGramSetDegree()
	{
		map<string, twoTuple> tg;
		map<string, twoTuple> ::iterator iter;
		
		for (int i = 0; i < v; i++)
		{
			stringInt s = branchGramVertexDegree(i);
			iter = tg.find(s.str);
			if (iter == tg.end())
			{
				tg.insert(pair<string, twoTuple>(s.str, twoTuple(1, s.degree)));
			}
			else
			{
				iter->second.row++;
			}
		}
		return tg;
	}

	string branchGramVertex(int id)
	{
		assert(id < v);
		int s = V[id];
		string branchSequence = common::inToString(s);
		vector<string> vs;
		for (int i = 0; i < v; i++)
		{
			if (Edge[id][i] != -1)
			{
				string temp = common::inToString(Edge[id][i]);
				vs.push_back(temp);
			}
		}
		sort(vs.begin(), vs.end());
		for (int i = 0; i < vs.size(); i++)
			branchSequence += vs[i];
		return branchSequence;
	}
	map<string, int> branchGramSet()
	{
		map<string, int> tg;
		map<string, int>::iterator iter;
		for (int i = 0; i < v; i++)
		{
			string s = branchGramVertex(i);
			iter = tg.find(s);
			if (iter == tg.end())
			{
				tg.insert(pair<string, int>(s, 1));
			}
			else
				iter->second++;
		}
		return tg;
	}

public:	
	map<string, int> vertexLabel()
	{
		map<string, int> mvl;
		map<string, int>::iterator iter;
		char ch[2];
		for (int i = 0; i < v; i++)
		{
			ch[0] = V[i]; 
			ch[1] = '\0';
			string str = ch;
			iter = mvl.find(str);
			if (iter == mvl.end())
				mvl.insert(pair<string, int>(str, 1));
			else
			{
				iter->second++;
			}
		}
		return mvl;
	}

	map<string, int> edgeLabel()
	{
		map<string, int> mvl;
		map<string, int>::iterator iter;
		char ch[2];

		for (int j = 0; j < v; j++)
		{
			for (int i = j + 1; i < v; i++)
			{
				if (Edge[j][i] != -1)
				{
					ch[0] = Edge[j][i];
					ch[1] = '\0';
					string einfo = ch;
					iter = mvl.find(einfo);
					if (iter == mvl.end())
						mvl.insert(pair<string, int>(einfo, 1));
					else
					{
						iter->second++;
					}
				}
			}
		}
		return mvl;
	}

	vector<int> degreeSequence(int &maxDegree)
	{
		vector<int> ds;
		maxDegree = 0;
		for (int i = 0; i < v; i++)
		{
			int degree = this->degreeVertex(i);
			ds.push_back(degree);
			if (maxDegree < degree)
				maxDegree = degree;
		}
		return ds;
	}

public:
	void vertexDeleteDegree(vector<int> &vr, vector<int> &degree) //delete the vertex set in vr 
	{
		for (int i = 0; i < v; i++)
		{
			if (common::inVector(vr, i))
				continue;
			int td = 0;
			for (int j = 0; j < v; j++)
			{
				if (j != i && Edge[i][j] != -1 && !common::inVector(vr, j))
					td++;
			}
			degree.push_back(td);
		}
	}

public:
	void generateInducedGraph(vector<int> &deleteVertices, graph &sub)
	{
		map<int, int> vertexMap;
		map<int, int>::iterator iter;
		int size = deleteVertices.size(), idx = 0, edge_num = 0;
		sub.v = this->v - size; sub.V.resize(sub.v, 0);
		vector<int> adj(sub.v, -1); sub.Edge.resize(sub.v, adj);
		vector<bool> flag(this->v, 0);

		for (int i = 0; i < size; i++)
			flag[deleteVertices[i]] = 1;
		for (int i = 0; i < this->v; i++)
		{
			if (!flag[i])
			{
				vertexMap.insert(pair<int, int>(i, idx));
				sub.V[idx] = this->V[i];
				idx++;
			}
		}
		for (int i = 0; i < this->v; i++)
		{		
			if (!flag[i])
			{
				iter = vertexMap.find(i);
				int from = iter->second;
				for (int j = i + 1; j < this->v; j++)
				{
					if (this->Edge[i][j] != -1 && !flag[j])
					{
						iter = vertexMap.find(j);
						int to = iter->second;
						sub.Edge[from][to] = this->Edge[i][j];
						sub.Edge[to][from] = this->Edge[i][j];
						edge_num++;
					}
				}

			}
		}
		sub.e = edge_num;
		sub.graph_id = -1; 
	}
public:
	void printf_graph()
	{
		cout << "gid:" << graph_id << endl;
		cout << "v, e:" << v << " " << e << endl;
		for(int i = 0; i < v; i++)
			cout << V[i] << endl;
		for(int i = 0; i < v; i++)
		{
			for(int j = 0; j < v;j++)
			{
				cout << Edge[i] [j] << " " ;
			}
			cout << endl;
		}
	}
};
#endif
