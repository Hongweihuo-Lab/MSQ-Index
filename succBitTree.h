#ifndef _SUCC_BITTREE_H
#define _SUCC_BITTREE_H

#include "stdafx.h"
#include "graph.h"
#include "branchHash.h"
#include "vertexHash.h"
#include "edgeHash.h"
#include "Node.h"
#include "branchTree.h"
#include "bitIndex.h"
#include "fixCode.h"

static uchar degree1[1024];
static uchar degree2[1024];
static uchar dq[1024]; 
static u32 count1;
static u32 count2;

static BTuple bt[1024];
static u32 countTuple;

typedef struct bitTreeOffset
{
	u64 offPos;
	u64 offHybrid;
	u64 offSample;
	u64 offGamma;
	u64 offWords;
	bitTreeOffset(u64 p, u64 h, u64 s, u64 g, u64 w) : 
		offPos(p), offHybrid(h), offSample(s), offGamma(g), offWords(w)
	{}
};

class succBitTree
{
public:
	struct succBitBranch
	{
		void *child; 
		u32 idx; 
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
		bool IsInternalNode()                         { return (m_level > 0); } 
		bool IsLeaf()									  { return (m_level == 0); } 
		int m_count;                                 
		int m_level;                                 
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
			sum += sizeof(int) * 2; 
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
	branchHash *bh;
	vertexHash *vh;
	edgeHash *eh;
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
		bh = branchHash::getInstance();
		vh = vertexHash::getInstance();
		eh = edgeHash::getInstance();
		sd = sv = se = NULL;
		fb = fl = fe = NULL;
		fve = NULL;
		totalGraphs = 0;
	}
	succBitTree(string tupleInfo)
	{
		root = NULL;
		bh = branchHash::getInstance();
		vh = vertexHash::getInstance();
		eh = edgeHash::getInstance();

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
		bh = branchHash::getInstance();
		vh = vertexHash::getInstance();
		eh = edgeHash::getInstance();
	}
	succBitTree(const sTuple &bt, const sTuple &vt, const sTuple &et, const u32 &maxve, const u64 &idx)
	{
		bh = branchHash::getInstance();
		vh = vertexHash::getInstance();
		eh = edgeHash::getInstance();

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
		if (!a_node) return; 
		if (a_node->IsInternalNode()) 
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
		map<int, int>::iterator iter;
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
	inline void Search(succBitNode* a_node, branch* a_rect, int d, vector<int> &result, graph &q)
	{
		ASSERT(a_node);
		ASSERT(a_node->m_level >= 0);
		ASSERT(a_rect);

		if (a_node->IsInternalNode()) // This is an internal node in the tree
		{

			for (int index = 0; index < a_node->m_count; ++index)
			{
				succBitNode *tmp = (succBitNode *)a_node->m_branch[index].child;
				if (this->ovelapSuccinctBitLabel(&a_node->m_branch[index], a_rect, d))
				{
					if (this->ovelapSuccinctBitBranch(&a_node->m_branch[index], a_rect, d))
						Search(tmp, a_rect, d, result, q);
				}
			}
		}
		else // This is a leaf node
		{
			for (int index = 0; index < a_node->m_count; ++index)
			{
				succBitBranch *tmp = &a_node->m_branch[index];
				u64 begin, end; this->fe->getValue(tmp->idx, begin, end);
				u64 vertex, edge; this->fve->getValue(tmp->idx, vertex, edge);
				int edgeSum = se->unionListSumInternal(a_rect->el, begin, end);

				if (edgeSum >= max(a_rect->edge, (int)edge) - d)  // if-1
				{
					this->fl->getValue(tmp->idx, begin, end);
					int vertexSum = sv->unionListSumInternal(a_rect->vl, begin, end);

					if (vertexSum + edgeSum >= max(a_rect->vertex, (int)vertex) + max(a_rect->edge, (int)edge) - d) // if-1.1
					{
						this->fb->getValue(tmp->idx, begin, end);
						sd->decodeLeafList(begin, end, bt, countTuple);
						int sum = a_rect->unionListSum(a_rect->bl, bt, countTuple);

						if (sum >= 2 * max(a_rect->vertex, (int)vertex) - vertexSum - 2 * d) //if-1.1.1
						{
							memcpy(dq, degree2, sizeof(uchar) * count2);
							bh->queryDegree(bt, countTuple, degree1, count1);
							int diff = count2 - count1; 
						
							if(diff <= 0)							
							{
								int ed = common::degreeEditDistance(degree1, count1, dq, count2);
								if (d >= max(a_rect->vertex, (int)vertex) - vertexSum + ed) //if-1.1.1.1.1
								{
									int *id = (int*)a_node->m_branch[index].child;
									result.push_back(*id);
								}//if-1.1.1.1.1
							}//if-1.1.1.1
							else //if-1.1.1.1
							{
								vector<int> dq1(degree2, degree2 + count2);
								vector<int> dg1(degree1, degree1 + count1);
								vector<int> idx;
								for (int i = 0; i < diff; i++)
									idx.push_back(i);
								vector<int> dqd; q.vertexDeleteDegree(idx, dqd);
								int dvmin = common::deleteEdge(dq1, dqd) + common::degreeEditDistance(dg1, dqd);
								while (true)
								{
									dqd.clear();
									bool flag = common::next_data(diff, dq1.size(), idx);
									if (!flag)  break;
									q.vertexDeleteDegree(idx, dqd);
									int tmp = common::deleteEdge(dq1, dqd) + common::degreeEditDistance(dg1, dqd);
									if (tmp < dvmin)
									{
										dvmin = tmp;
									}
								}
								int ed = dvmin;
								if (d >= max(a_rect->vertex, (int)vertex) - vertexSum + ed) // if-1.1.1.1.1
								{
									int *id = (int*)a_node->m_branch[index].child;
									result.push_back(*id);
								}//if-1.1.1.1.1
							}//else
						}//if-1.1.1
					}//if-1.1
				}//if-1
			}//for
		}//
	}	
public:
		succBitNode *readNode(vector<int> &vbt, bitTreeOffset &bt, vector<int> &vvt, bitTreeOffset &vt, vector<int> &vet, bitTreeOffset &et,
			FILE *&fr, u64 &idx)
		{
			succBitNode *newNode = new succBitNode();
			fread(&newNode->m_level, sizeof(int), 1, fr);
			fread(&newNode->m_count, sizeof(int), 1, fr);
			newNode->m_branch = new succBitBranch[newNode->m_count];
			bool f = newNode->m_level == 0;
			int vertex, c, v, length, edge;
			u64 offBit;
			u64 pos;

			for (int i = 0; i < newNode->m_count; i++)
			{
				if (f) 
				{
					newNode->m_branch[i].child = new int();
					fread(newNode->m_branch[i].child, sizeof(int), 1, fr); // the graph ID in the db
				}

				BList bl; // stack 
				fread(&vertex, sizeof(int), 1, fr);
				fread(&edge, sizeof(int), 1, fr);
				fread(&length, sizeof(int), 1, fr);
			
				newNode->m_branch[i].idx = idx++;
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
		inline vector<int> queryGraph(graph &q, int d)
		{
			branch *bg = convertRect(q);
			vector<int> result;
			this->Search(root, bg, d, result, q);
 			if (bg) delete bg;
			return result;
		}
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
			sum = this->indexSize(this->root);
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
};

#endif
