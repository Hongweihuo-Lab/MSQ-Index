#ifndef _SUCC_BITTREE_H
#define _SUCC_BITTREE_H

#include "graph.h"
#include "globalHash.h"
#include "Node.h"
#include "queryRect.h"
#include "branchTree.h"
#include "bitIndex.h"
#include "fixCode.h"

static int degree1[1024];
static int degree2[1024];
static int dq[1024]; //copy
static int count1;
static int count2;
static BTuple bt[1024];

static int countTuple;

class succBitTree
{
public:
	struct succBitBranch
	{
		void *child; //it is Node or graphId information
		u32 idx; // 
		succBitBranch()
		{
			idx = 0;
			child = NULL;
		}
		u32 succBitBranchMem()
		{
			u32 sum = 0;
			sum += sizeof(idx);
			sum += sizeof(void *);
			return sum;
		}
	};
	struct succBitNode
	{
		bool IsInternalNode()                         { return (m_level > 0); } // Not a leaf, but a internal node
		bool IsLeaf()									  { return (m_level == 0); } // A leaf, contains data
		int m_count;                                  ///< Count
		int m_level;                                  ///< Leaf is zero, others positive
		succBitBranch *m_branch;

		succBitNode()
		{
			m_count = 0;
			m_level = 0;
			m_branch = NULL;
		}

		u32 succBitNodeMem()
		{
			u32 sum = 0;
			sum += sizeof(int) * 2; //m_count and m_leaf
			for (int i = 0; i < this->m_count; i++)
				sum += this->m_branch[i].succBitBranchMem();
			return sum;
		}
		~succBitNode()
		{
			if (m_branch) delete[] m_branch;
		}
	};

public:
	succBitNode *root;
	bitIndex *sd;
	bitIndex *sv;
	bitIndex *se;

	fixCode *fve;
	fixCode *fb;
	fixCode *fl;
	fixCode *fe;

	u32 totalGraphs; 
public:
	succBitTree()
	{
		root = NULL;
		sd = sv = se = NULL;
		fb = fl = fe = NULL;
		fve = NULL;
		totalGraphs = 0;
	}
	succBitTree(string tupleInfo)
	{
		root = NULL;

		FILE *fr = fopen(tupleInfo.c_str(), "rb"); assert(fr);
		sTuple bt, vt, et;
		u64 idx;
		u32 maxve;
		fread(&bt, sizeof(sTuple), 1, fr);
		fread(&vt, sizeof(sTuple), 1, fr);
		fread(&et, sizeof(sTuple), 1, fr);
		fread(&idx, sizeof(u64), 1, fr);
		fread(&maxve, sizeof(u32), 1, fr);
		fread(&totalGraphs, sizeof(u32), 1, fr);		
		if (fr) fclose(fr); fr = NULL;

		this->sd = new bitIndex(bt, BBS);
		this->sv = new bitIndex(vt, VBS);
		this->se = new bitIndex(et, EBS);

		this->fb = new fixCode(bt.bitLen, 2 * idx);
		this->fl = new fixCode(vt.bitLen, 2 * idx);
		this->fe = new fixCode(et.bitLen, 2 * idx);
		this->fve = new fixCode(maxve, 2 * idx);
	}
	succBitTree(const sTuple &bt, const sTuple &vt, const sTuple &et)
	{
		sd = new bitIndex(bt, BBS);
		sv = new bitIndex(vt, VBS);
		se = new bitIndex(et, EBS);
		root = NULL;
	}
	succBitTree(const sTuple &bt, const sTuple &vt, const sTuple &et, const u32 &maxve, const u64 &idx)
	{
	
		sd = new bitIndex(bt, BBS);
		sv = new bitIndex(vt, VBS);
		se = new bitIndex(et, EBS);
		this->fb = new fixCode(bt.bitLen, 2 * idx);
		this->fl = new fixCode(vt.bitLen, 2 * idx);
		this->fe = new fixCode(et.bitLen, 2 * idx);
		this->fve = new fixCode(maxve, 2 * idx);
		root = NULL;
		
	}
public:

	void FreeNode(succBitNode *a_node)
	{

		if (a_node) delete a_node;
		a_node = NULL;
	}
	void RemoveAllRec(succBitNode* a_node)
	{
		if (!a_node) return; //the tree is empty
		if (a_node->IsInternalNode()) // This is an internal node in the tree
		{
			for (int index = 0; index < a_node->m_count; ++index)
			{
				RemoveAllRec((succBitNode *)a_node->m_branch[index].child);
			}
			FreeNode(a_node);
		}
		else
		{
			for (int index = 0; index < a_node->m_count; ++index)
			{
				if (a_node->m_branch[index].child)
					delete a_node->m_branch[index].child;
			}
			FreeNode(a_node);
		}
	}
	~succBitTree()
	{
		RemoveAllRec(root);
		root = NULL;
		if (sd) delete sd;
		if (sv) delete sv;
		if (se) delete se;
		if (fb) delete fb;
		if (fl) delete fl;
		if (fe) delete fe;
}
public:	
	inline branch* convertRect(graph &g)
	{
		map<string, twoTuple> fs = g.branchGramSetDegree();
		map<int, int> mv = bh->queryVector(fs);
		bh->queryDegree(mv, degree2, count2);

		map<string, int> vl = g.vertexLabel();
		map<int, int> mvl = vh->queryVector(vl);
		map<string, int> el = g.edgeLabel();
		map<int, int> mel = eh->queryVector(el);		
		branch *a_rect = new branch(mv, mvl, mel, g.v, g.e);
		return a_rect;
	}
	inline bool ovelapSuccinctBitLabel(succBitBranch *a_rect, branch* b_rect, int d)
	{
		int sum = 0;
		u64 begin, end;
		this->fe->getValue(a_rect->idx, begin, end);
		sum += se->unionListSumInternal(b_rect->el, begin, end);
		u64 edge, vertex; this->fve->getValue(a_rect->idx, vertex, edge); 
		if (sum >= max(int(edge), b_rect->edge) - d)
		{
			this->fl->getValue(a_rect->idx, begin, end); // problem is here 
			sum += sv->unionListSumInternal(b_rect->vl, begin, end);
			if (sum >= max((int)vertex, b_rect->vertex) + max((int)edge, b_rect->edge) - d)
				return true;
			else
				return  false;
		}
		else
			return false;
	}
	inline bool ovelapSuccinctBitBranch(succBitBranch *a_rect, branch* b_rect, int d)
	{
		int sum;
		u64 begin, end;
		this->fb->getValue(a_rect->idx, begin, end);
		sum = sd->unionListSumInternal(b_rect->bl, begin, end);
		u64 vertex, edge; this->fve->getValue(a_rect->idx, vertex, edge);
		if (sum >= max((int)vertex, b_rect->vertex) - 2 * d)
			return true;
		else
			return false;
	}	
	public:
		succBitNode *readNode(vector<int> &vbt, bitTreeOffset &bt, vector<int> &vvt, bitTreeOffset &vt, vector<int> &vet, bitTreeOffset &et,
			FILE *&fr, u64 &idx)
		{
			succBitNode *newNode = new succBitNode();
			fread(&newNode->m_level, sizeof(int), 1, fr);
			fread(&newNode->m_count, sizeof(int), 1, fr);
			newNode->m_branch = new succBitBranch[newNode->m_count];
			bool f = newNode->m_level == 0; //if it is a leaf node, f is true, otherwise, it is a internal node.
			int vertex, c, v, length, edge;
			u64 offBit;
			u64 pos;

			for (int i = 0; i < newNode->m_count; i++)
			{
				if (f) // it is a leaf node 
				{
					newNode->m_branch[i].child = new int();
					fread(newNode->m_branch[i].child, sizeof(int), 1, fr); // the graph ID in the db
				}

				BList bl; // stack 
				fread(&vertex, sizeof(int), 1, fr);
				fread(&edge, sizeof(int), 1, fr);
				fread(&length, sizeof(int), 1, fr);
			
				newNode->m_branch[i].idx = idx++;
				//newNode->m_branch[i].vertex = vertex;
				//newNode->m_branch[i].edge = edge;

				pos = newNode->m_branch[i].idx;
				pos = 2 * pos * fve->words;
				bitOperation::setbit(fve->bit, pos, vertex, fve->words);
				bitOperation::setbit(fve->bit, pos + fve->words, edge, fve->words);

				int max = 0;
				offBit = bt.offPos;
				for (int j = 0; j < length; j++)
				{
					fread(&c, sizeof(int), 1, fr);
					fread(&v, sizeof(int), 1, fr);
					if (max < c)
						max = c;
					bitOperation::setbit(sd->posRank->bit, bt.offPos + c, 1, 1);
					if (vbt.size() == BBS)
					{
						for(int k = 0; k < vbt.size();k++)
						{
							map<int, int>::iterator iter = MD.find(vbt[k]);
							if(iter != MD.end())
							{
								iter->second++;
							}
							else
							{
								MD.insert(pair<int, int>(vbt[k], 1));
							}
						}
						if (bitOperation::fixMoreGamma(vbt))
						{
							sd->encodeGama(vbt, bt.offHybrid, bt.offSample, bt.offGamma);
						}
						else
						{
							sd->encodeFix(vbt, bt.offHybrid, bt.offSample, bt.offWords);
						}
						vbt.clear();
					}
					vbt.push_back(v);
				}
				bt.offPos += max + 1;
				
				pos = newNode->m_branch[i].idx;
				pos = pos * 2 * fb->words;
				bitOperation::setbit(fb->bit, pos, offBit, fb->words);
				bitOperation::setbit(fb->bit, pos + fb->words, bt.offPos - 1, fb->words);

				max = 0;
				offBit = vt.offPos;
				fread(&length, sizeof(int), 1, fr);
				for (int j = 0; j < length; j++)
				{
					fread(&c, sizeof(int), 1, fr);
					fread(&v, sizeof(int), 1, fr);
					if (max < c)
						max = c;
					bitOperation::setbit(sv->posRank->bit, vt.offPos + c, 1, 1);
					if (vvt.size() == VBS)
					{
						for(int k = 0; k < vvt.size();k++)
						{
							map<int, int>::iterator iter = ML.find(vvt[k]);
							if(iter != ML.end())
							{
								iter->second++;
							}
							else
							{
								ML.insert(pair<int, int>(vvt[k], 1));
							}
						}
						if (bitOperation::fixMoreGamma(vvt))
						{
							sv->encodeGama(vvt, vt.offHybrid, vt.offSample, vt.offGamma);
						}
						else
						{
							sv->encodeFix(vvt, vt.offHybrid, vt.offSample, vt.offWords);
						}
						vvt.clear();
					}
					vvt.push_back(v);
				}
				vt.offPos += max + 1;

				pos = newNode->m_branch[i].idx;
				pos = pos * 2 * fl->words;
				bitOperation::setbit(fl->bit, pos, offBit, fl->words);
				bitOperation::setbit(fl->bit, pos + fl->words, vt.offPos - 1, fl->words);
				
				max = 0;
				offBit = et.offPos;
				fread(&length, sizeof(int), 1, fr);
				for (int j = 0; j < length; j++)
				{
					fread(&c, sizeof(int), 1, fr);
					fread(&v, sizeof(int), 1, fr);
					if (max < c)
						max = c;
					bitOperation::setbit(se->posRank->bit, et.offPos + c, 1, 1);
					if (vet.size() == EBS)
					{
						for(int k = 0; k < vet.size(); k++)
						{
							map<int, int>::iterator iter = ML.find(vet[k]);
							if(iter != ML.end())
							{
								iter->second++;
							}
							else
							{
								ML.insert(pair<int, int>(vet[k], 1));
							}
						}
						if (bitOperation::fixMoreGamma(vet))
						{
							se->encodeGama(vet, et.offHybrid, et.offSample, et.offGamma);
						}
						else
						{
							se->encodeFix(vet, et.offHybrid, et.offSample, et.offWords);
						}
						vet.clear();
					}
					vet.push_back(v);
				}
				et.offPos += max + 1;

				pos = newNode->m_branch[i].idx;
				pos = 2 * pos * fe->words;
				bitOperation::setbit(fe->bit, pos, offBit, fe->words);
				bitOperation::setbit(fe->bit, pos + fe->words, et.offPos - 1, fe->words);
			
			}
			return newNode;
		}

		void readSuccTree(succBitNode **a_node, vector<int> &vbt, bitTreeOffset &bt, vector<int> &vvt, bitTreeOffset &vt, vector<int> &vet,
			bitTreeOffset &et, FILE *&fr, u64 &idx)
		{

			*a_node = readNode(vbt, bt, vvt, vt, vet, et, fr, idx);
			if ((*a_node)->IsInternalNode())
			{
				for (int j = 0; j < (*a_node)->m_count; j++)
				{
					readSuccTree((succBitNode **)&(*a_node)->m_branch[j].child, vbt, bt, vvt, vt, vet, et, fr, idx);
				}
			}
		}


		void buildSuccBitTree(const string path)
		{
			FILE *fr = fopen(path.c_str(), "rb+"); assert(fr);
			bitTreeOffset bt(0, 0, 0, 0, 0);
			bitTreeOffset vt(0, 0, 0, 0, 0);
			bitTreeOffset et(0, 0, 0, 0, 0);
			vector<int> vbt; vector<int> vvt; vector<int> vet;
			u64 idx = 0;

			this->readSuccTree(&this->root, vbt, bt, vvt, vt, vet, et, fr, idx);		
			if (vbt.size() > 0)
			{
				for(int i = 0; i < vbt.size();i++)
				{
					map<int, int>::iterator iter = MD.find(vbt[i]);
					if(iter != MD.end())
					{
						iter->second++;
					}
					else
					{
						MD.insert(pair<int, int>(vbt[i], 1));
					}
				}
				if (bitOperation::fixMoreGamma(vbt))
				{
					sd->encodeGama(vbt, bt.offHybrid, bt.offSample, bt.offGamma);
				}
				else
				{
					sd->encodeFix(vbt, bt.offHybrid, bt.offSample, bt.offWords);
				}
				vbt.clear();
			}
			if (vvt.size() > 0)
			{
				for(int i = 0; i < vvt.size();i++)
				{
					map<int, int>::iterator iter = ML.find(vvt[i]);
					if(iter != ML.end())
					{
						iter->second++;
					}
					else
					{
						ML.insert(pair<int, int>(vvt[i], 1));
					}
				}
				if (bitOperation::fixMoreGamma(vvt))
				{
					sv->encodeGama(vvt, vt.offHybrid, vt.offSample, vt.offGamma);
				}
				else
				{
					sv->encodeFix(vvt, vt.offHybrid, vt.offSample, vt.offWords);
				}
				vvt.clear();
			}
			if (vet.size()> 0)
			{
				for(int i = 0; i < vet.size();i++)
				{
					map<int, int>::iterator iter = ML.find(vet[i]);
					if(iter != ML.end())
					{
						iter->second++;
					}
					else
					{
						ML.insert(pair<int, int>(vet[i], 1));
					}
				}
				if (bitOperation::fixMoreGamma(vet))
				{
					se->encodeGama(vet, et.offHybrid, et.offSample, et.offGamma);
				}
				else
				{
					se->encodeFix(vet, et.offHybrid, et.offSample, et.offWords);
				}
				vet.clear();
			}

			this->sd->posRank->initRankTable(); this->sd->flagRank->initRankTable();
			this->sv->posRank->initRankTable(); this->sv->flagRank->initRankTable();
			this->se->posRank->initRankTable(); this->se->flagRank->initRankTable();
			if (fr) fclose(fr); fr = NULL;
		}

public:		
		int indexSize(succBitNode *a_node)
		{
			ASSERT(a_node);
			int sum = a_node->succBitNodeMem();
			if (!a_node->IsInternalNode())
			{
				return sum;
			}
			else
			{
				for (int index = 0; index < a_node->m_count; ++index)
				{
					sum += indexSize((succBitNode *)a_node->m_branch[index].child);
				}
			}
			return sum;
		}
		double succinctBitTreeSize()
		{
			u32 sum = 0;
			double size = 0.0;
			
			SD.T_X += this->sd->indexSize;
			SV.T_X += this->sv->indexSize;
			SE.T_X += this->se->indexSize;			
						
			sum = this->indexSize(this->root);
			Ma += sum; 
	
			sum += this->sd->indexSize;
			sum += this->sv->indexSize;
			sum += this->se->indexSize;
			sum += this->fb->size;  //Byte
			sum += this->fl->size;
			sum += this->fe->size;
			sum += this->fve->size;
			size = sum * 1.0 / (1024.0 * 1024.0);
			return size;
		}
		void succinctBitTreeSize(u32 &ve, u32 &ble, u32 &lr, u32 &others)
		{
			ve += this->fve->size;
			ble += this->sd->indexSize + this->sv->indexSize + this->se->indexSize;
			lr += this->fb->size + this->fl->size + this->fe->size;
			others += this->indexSize(this->root);
		}

public:

	inline vector<int> boostedQuery(vector<vector<queryRect * > > &q_subsets, int &d)
	{
		vector<int> result;			
		this->boostedSearch(root, q_subsets, d, result);
		return result; 
	}
	bool filteredOut(succBitBranch * tmp, queryRect *q, int d, int ve)
	{

		d = d - ve;
		int edgeSum = se->unionListSumInternal(q->rect->el, g_edge_beign, g_edge_end);
		branch *a_rect = q->rect;

		if (edgeSum >= max(a_rect->edge, (int)g_edge) - d)  
		{
			int vertexSum = sv->unionListSumInternal(a_rect->vl, g_vertex_begin, g_vertex_end);

			if (vertexSum + edgeSum >= max(a_rect->vertex, (int)g_vertex) 
				+ max(a_rect->edge, (int)g_edge) - d) // label-based q-gram counting filter
			{
				int sum = a_rect->unionListSum(a_rect->bl, g_branch_tuple, g_branch_count);
				if (sum >= 2 * max(a_rect->vertex, (int)g_vertex) - vertexSum - 2 * d) //branch-based q-gram counting filter 
				{
					int EI, ED;
					common::degreeEditDistance(g_degree, g_vertex, q->degree, a_rect->vertex, EI, ED);
					int ed = EI + ED;
					if (d >= max(a_rect->vertex, (int)g_vertex) - vertexSum + ed
						&& d >= max(a_rect->vertex, (int)g_vertex) - vertexSum +
						a_rect->edge - edgeSum + ED) //degree-based filter 
					{
						return 0;
					}//if-1.1.1.1.1
				}
			}
		}
		return 1;
	}

	inline void boostedSearch(succBitNode* a_node, vector<vector<queryRect * > >
		&q_subgraphs_sets, int &d, vector<int> &result)
	{
		ASSERT(a_node);
		ASSERT(a_node->m_level >= 0);
		
		branch *a_rect = q_subgraphs_sets[0][0]->rect;
		if (a_node->IsInternalNode()) // This is an internal node in the tree
		{

			for (int index = 0; index < a_node->m_count; ++index)
			{
				succBitNode *tmp = (succBitNode *)a_node->m_branch[index].child;
				if (this->ovelapSuccinctBitLabel(&a_node->m_branch[index], a_rect, d))
				{
					if (this->ovelapSuccinctBitBranch(&a_node->m_branch[index], a_rect, d))
						boostedSearch(tmp, q_subgraphs_sets, d, result);
				}
			}
		}
		else // This is a leaf node
		{
			int k = q_subgraphs_sets.size() - 1;
			for (int index = 0; index < a_node->m_count; ++index) //each represents a data graoh 
			{
				succBitBranch *tmp = &a_node->m_branch[index];
				this->fe->getValue(tmp->idx, g_edge_beign, g_edge_end);
				this->fve->getValue(tmp->idx, g_vertex, g_edge);				
				int edgeSum = se->unionListSumInternal(a_rect->el, g_edge_beign, g_edge_end);				
				
				if (edgeSum >= max(a_rect->edge, (int)g_edge) - d)  
				{
					this->fl->getValue(tmp->idx, g_vertex_begin, g_vertex_end);					
					int vertexSum = sv->unionListSumInternal(a_rect->vl, g_vertex_begin, g_vertex_end);
					if (vertexSum + edgeSum >= max(a_rect->vertex, (int)g_vertex) 
						+ max(a_rect->edge, (int)g_edge) - d) // label-based q-gram counting filter
					{						
						this->fb->getValue(tmp->idx, g_branch_begin, g_branch_end);		
						this->sd->decodeLeafList(g_branch_begin, g_branch_end, g_branch_tuple, g_branch_count);
						int sum = a_rect->unionListSum(a_rect->bl, g_branch_tuple, g_branch_count);
						if (sum >= 2 * max(a_rect->vertex, (int)g_vertex) - vertexSum - 2 * d) //branch-based q-gram counting filter 
						{
							bh->queryDegree(g_branch_tuple, g_branch_count, g_degree, g_count);
							sort(g_degree, g_degree + (int)g_count, greater<int>()); 	
							assert(g_count == g_vertex);
							int diff = a_rect->vertex - g_vertex;

							if (diff <= 0 ||(k == 0) )
							{
								int EI, ED;
								common::degreeEditDistance(g_degree,  g_vertex,  q_subgraphs_sets[0][0]->degree,  a_rect->vertex, EI, ED);
								int ed = EI + ED;								
								if (d >= max(a_rect->vertex, (int)g_vertex) - vertexSum + ed
									&& 	d >= max(a_rect->vertex, (int)g_vertex) - vertexSum 
									+ a_rect->edge - edgeSum + ED)
									//degree-based filter, with wei Guo's Filter 
								{
									int *id = (int*)a_node->m_branch[index].child;
									result.push_back(*id);
								}//if-1.1.1.1.1
							}//if-1.1.1.1
							else //if-1.1.1.1
							{
								assert(k >= 1);
								bool flag = 0;
								k = min((int)(a_rect->vertex - g_vertex), k);
								for (int i = 1; i <= k; i++)								
								{								
									bool flag_layer = 0;
									int size = q_subgraphs_sets[i].size();
									for (int j = 0; j < size; j++)
									{										
										queryRect *qg = q_subgraphs_sets[i][j];
										int ve = a_rect->vertex + a_rect->edge - qg->rect->vertex - qg->rect->edge;
										if (!filteredOut(tmp, qg, d, ve)) //filterd 
										{
											flag_layer = 1;
											break;
										}
									}
									if (flag_layer == 0)
									{
										flag = 1;
										break;
									}
								}
								if (flag == 0)
								{
									int *id = (int*)a_node->m_branch[index].child;
									result.push_back(*id);
								}
							}//else
						}//if-1.1.1
					}//if-1.1
				}//if-1
			}//for
		}//

	}

};

#endif
