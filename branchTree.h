#ifndef _BRANCH_TREE
#define _BRANCH_TREE

#include "branch.h"
#include "graph.h"
#include "Node.h"
#include "globalHash.h"

class branchTree
{
public:
	struct Paration
	{
		int m_partition[MAXNODES + 1];
		int m_total;
		branch m_branchBuf[MAXNODES + 1];
	};
	Node *root;
	u32 totalGraphs;

public:
	branchTree()
	{
		root = new Node();
		totalGraphs = 0;
	}
	Node* AllocNode()
	{
		Node* newNode = new Node();
		InitNode(newNode);
		return newNode;
	}

	void InitNode(Node *a_node)
	{
		a_node->m_count = 0;
		a_node->m_level = 0;
	}

	bool OverlapLabel(branch* a_rect, branch* b_rect, int d)
	{
		int sum = 0;
		sum += a_rect->unionListSum(a_rect->vl, b_rect->vl); //vl
		if (sum >= max(a_rect->vertex, b_rect->vertex) - d)
		{
			sum += a_rect->unionListSum(a_rect->el, b_rect->el); //el
			if (sum >= max(a_rect->vertex, b_rect->vertex) + max(a_rect->edge, b_rect->edge) - d)
				return true;
			else
				return false;
		}
		else
			return false;
	}

	bool OverlapBranch(branch* a_rect, branch* b_rect, int d)
	{
		int sum = 0;
		sum += a_rect->unionListSum(a_rect->bl, b_rect->bl);
		if (sum >= a_rect->vertex - 2 * d && sum >= b_rect->vertex - 2 * d)
			return true;
		else
			return false;
	}

	void  CombineRect(branch* a_rectA, branch* a_rectB, branch &ct)
	{
		//don't response for the space release
		assert(a_rectA && a_rectB);
		ct.clear();
		BTuple bt;
		BList *temp;
		temp = a_rectA->mergeList(a_rectA->bl, a_rectB->bl);
		*ct.bl = *temp;  if (temp) delete temp;
		temp = a_rectA->mergeList(a_rectA->vl, a_rectB->vl);
		*ct.vl = *temp; if (temp) delete temp;
		temp = a_rectA->mergeList(a_rectA->el, a_rectB->el);
		*ct.el = *temp; if (temp) delete temp;

		ct.vertex = min(a_rectA->vertex, a_rectB->vertex);
		ct.edge = min(a_rectA->edge, a_rectB->edge);

	}

	void  NodeCover(Node* node, branch &rect)
	{
		assert(node);
		branch tmp;
		tmp = node->m_branch[0]; //deep copy
		for (int index = 1; index < node->m_count; ++index)
		{
			CombineRect(&tmp, &(node->m_branch[index]), rect);
			tmp = rect;
		}
	}
	void  AddBranch(branch* a_branch, Node** a_node)
	{
		assert(a_branch);
		ASSERT(*a_node);
		ASSERT((*a_node)->m_count < MAXNODES);
		(*a_node)->m_branch[(*a_node)->m_count] = *a_branch; //deep copy
		++(*a_node)->m_count;
	}
	void DisconnectBranch(Node * a_node, int a_index)
	{
		assert(a_node && (a_index >= 0) && (a_index < MAXNODES));
		assert(a_node->m_count > 0);
		a_node->m_branch[a_index] = a_node->m_branch[a_node->m_count - 1];
		--a_node->m_count;
	}
	//the split occur in the a_node
	//adopt the standard methods to split node
	void GetBranches(Node* a_node, branch* a_branch, Paration* a_parVars)
	{
		assert(a_node);
		assert(a_branch);
		assert(a_node->m_count == MAXNODES);
		for (int index = 0; index < MAXNODES; ++index)
		{
			a_parVars->m_branchBuf[index] = a_node->m_branch[index];
		}
		a_parVars->m_branchBuf[MAXNODES] = *a_branch;
		a_parVars->m_total = MAXNODES + 1;

		for (int index = 0; index < MAXNODES + 1; ++index)
		{
			a_parVars->m_partition[index] = -1;
		}
		InitNode(a_node);
	}
	//from the paration points to get two nodes: a_nodeA, a_nodeB
	void LoadNodes(Node* a_nodeA, Node* a_nodeB, Paration * a_parVars)
	{
		assert(a_nodeA);
		assert(a_nodeB);
		assert(a_parVars);
		for (int index = 0; index < a_parVars->m_total; ++index)
		{
			assert(a_parVars->m_partition[index] == 0 || a_parVars->m_partition[index] == 1);

			if (a_parVars->m_partition[index] == 0)
			{
				AddBranch(&a_parVars->m_branchBuf[index], &a_nodeA);
			}
			else if (a_parVars->m_partition[index] == 1)
			{
				AddBranch(&a_parVars->m_branchBuf[index], &a_nodeB);
			}
		}
	}
	// get the sum of the branch:  rectVolume
	//  
	int rectVolume(branch* a_rect, branch *b_rect)
	{
		int sum = 0;
		sum += a_rect->unionListSum(a_rect->bl, b_rect->bl);
		sum += a_rect->unionListSum(a_rect->vl, b_rect->vl);
		sum += a_rect->unionListSum(a_rect->el, b_rect->el);
		return sum;
	}

	int PickBranch(Node* a_node, branch* a_rect)
	{
		assert(a_rect && a_node);
		bool firstTime = true;
		int increase;
		int bestIncr = -1;
		int bestVertex;
		int vertex;
		int best;

		for (int index = 0; index < a_node->m_count; ++index)
		{
			branch* curRect = &a_node->m_branch[index];
			increase = rectVolume(curRect, a_rect);
			vertex = curRect->vertex;

			if ((bestIncr < increase) || firstTime)
			{
				best = index;
				bestIncr = increase;
				firstTime = false;
				bestVertex = curRect->vertex;
			}
			else if ((increase == bestIncr) && (bestVertex < vertex))
			{
				best = index;
				bestVertex = vertex;
				bestIncr = increase;
			}
		}
		return best;
	}
	void randomDivide(Paration *&a_parVars, int &c1, int &c2, vector<int> &g1, vector<int> &g2)
	{
		srand(time(0));
		c1 = rand() % (MAXNODES + 1);  // the random point n0
		int min = 10000;
		for (int i = 0; i <= MAXNODES; i++)
		{
			int rectValue = rectVolume(&a_parVars->m_branchBuf[i], &a_parVars->m_branchBuf[c1]);
			if (rectValue < min && i != c1)
			{
				min = rectValue;
				c2 = i; // the point i
			}
		}
		//ok, c1 and c2 choose done
		//then, divide the group into two parts
		g1.clear(); g2.clear();
		for (int i = 0; i < MAXNODES; i++)
		{
			if (i == c1)
			{
				g1.push_back(i);
				continue;
			}
			if (i == c2)
			{
				g2.push_back(i);
				continue;
			}
			int rect1 = rectVolume(&a_parVars->m_branchBuf[i], &a_parVars->m_branchBuf[c1]);
			int rect2 = rectVolume(&a_parVars->m_branchBuf[i], &a_parVars->m_branchBuf[c2]);
			if (rect1 < rect2)
			{
				g2.push_back(i);
				continue;
			}
			else
				g1.push_back(i);
		}
	}
	//assuming that: |g1| > |g2|
	void refixTheGroup(Paration *&a_parVars, int &c2, vector<int> &g1, vector<int> &g2)
	{
		vector<twoTuple> tmp;
		for (int i = 0; i < g1.size(); i++)
		{
			int rect = -rectVolume(&a_parVars->m_branchBuf[g1[i]], &a_parVars->m_branchBuf[c2]);
			tmp.push_back(twoTuple(rect, g1[i]));
		}
		sort(tmp.begin(), tmp.end());

		for (int j = 0; j < a_parVars->m_total / 2 - g2.size(); j++)
		{
			a_parVars->m_partition[tmp[j].value] = 1; // 属于第二组
		}
	}
	void ChooseParitionPoint(Paration *&a_parVars)
	{
		assert(a_parVars);
		assert(a_parVars->m_total == MAXNODES + 1);
		int c1, c2;
		vector<int> g1, g2;
		int n = a_parVars->m_total;
		for (int j = 0; j < n; j++)
			a_parVars->m_partition[j] = 0; // the first group
		randomDivide(a_parVars, c1, c2, g1, g2);

		int sz1 = g1.size(), sz2 = g2.size();
		if (sz1 == sz2)
		{
			for (int j = 0; j < sz2; j++)
				a_parVars->m_partition[g2[j]] = 1;
			return;
		}
		else if (sz1 > sz2) //the first group is larger than the other
		{
			for (int j = 0; j < sz2; j++)
				a_parVars->m_partition[g2[j]] = 1;
			refixTheGroup(a_parVars, c2, g1, g2);
			return;
		}
		else
		{
			for (int j = 0; j < sz1; j++)
				a_parVars->m_partition[g1[j]] = 1;
			refixTheGroup(a_parVars, c1, g2, g1);
			return;
		}
	}
	bool AddBranch(branch* a_branch, Node* a_node, Node** a_newNode)
	{
		ASSERT(a_branch);
		ASSERT(a_node);

		if (a_node->m_count < MAXNODES)  // Split won't be necessary
		{
			a_node->m_branch[a_node->m_count] = *a_branch;
			++a_node->m_count;
			return false;
		}
		else
		{
			ASSERT(a_newNode);
			SplitNode(a_node, a_branch, a_newNode);
			return true;
		}
	}
	void SplitNode(Node* a_node, branch* a_branch, Node** a_newNode)
	{
		ASSERT(a_node);
		ASSERT(a_branch);

		Paration localVars;
		Paration* parVars = &localVars;
		int level;
		level = a_node->m_level;
		GetBranches(a_node, a_branch, parVars);
		ChooseParitionPoint(parVars);
		*a_newNode = AllocNode();
		(*a_newNode)->m_level = a_node->m_level = level;
		LoadNodes(a_node, *a_newNode, parVars);

		ASSERT((a_node->m_count + (*a_newNode)->m_count) == parVars->m_total);
	}
	//convert the branch
	//here: must optimise it
	branch* convertRect(graph &g)
	{
		map<string, twoTuple> fs = g.branchGramSetDegree();
		map<int, int> mv = bh->queryVector(fs);
		map<string, int> vl = g.vertexLabel();
		map<int, int> mvl = vh->queryVector(vl);
		map<string, int> el = g.edgeLabel();
		map<int, int> mel = eh->queryVector(el);
		map<int, int>::iterator iter;
		branch *a_rect = new branch(mv, mvl, mel, g.v, g.e);
		return a_rect;
	}
	void Insert(graph &g)
	{
		branch *a_rect = convertRect(g);
		Insert(a_rect, g.graph_id);
		if (a_rect) delete a_rect;
		this->totalGraphs++;
	}
	void Insert(branch* a_rect, int a_dataId)
	{
		InsertRect(a_rect, a_dataId, &root, 0);
	}
	//information diushi  
	bool InsertRectRec(branch* a_rect, int a_id, Node *a_node, Node **a_newNode, int a_level)
	{
		ASSERT(a_rect &&  a_node);
		int index;
		branch bh;
		Node* otherNode;

		if (a_node->m_level > a_level)
		{
			index = PickBranch(a_node, a_rect);

			if (!InsertRectRec(a_rect, a_id, (Node *)a_node->m_branch[index].child, &otherNode, a_level))
			{
				Node *child = (Node *)a_node->m_branch[index].child;
				CombineRect(a_rect, &(a_node->m_branch[index]), bh);
				a_node->m_branch[index] = bh;
				a_node->m_branch[index].child = child;
				return false;
			}
			else
			{
				Node *child = (Node *)a_node->m_branch[index].child;
				NodeCover((Node *)a_node->m_branch[index].child, bh);
				a_node->m_branch[index] = bh;
				a_node->m_branch[index].child = child;

				NodeCover(otherNode, bh); //the space will be 
				bh.child = otherNode; //what ?
				return AddBranch(&bh, a_node, a_newNode);
			}
		}
		else if (a_node->m_level == a_level) //the insert level
		{
			bh = *a_rect;
			bh.child = new int(a_id);
			bool flag = AddBranch(&bh, a_node, a_newNode);
			return flag;
		}
		else
		{
			ASSERT(0);
			return false;
		}
	}
	bool InsertRect(branch* a_rect, const int a_id, Node** a_root, int a_level)
	{
		ASSERT(a_rect && a_root);
		ASSERT(a_level >= 0 && a_level <= (*a_root)->m_level);

		Node* newRoot;
		Node* newNode;
		branch bh;

		if (InsertRectRec(a_rect, a_id, *a_root, &newNode, a_level))  // Root split
		{
			newRoot = AllocNode();  // Grow tree taller and new root
			newRoot->m_level = (*a_root)->m_level + 1;
			NodeCover(*a_root, bh);
			bh.child = *a_root;
			AddBranch(&bh, newRoot, NULL);

			NodeCover(newNode, bh);
			bh.child = newNode;
			AddBranch(&bh, newRoot, NULL);
			*a_root = newRoot;
			return true;
		}
		return false;
	}	
public:
	void nodeBitGamaLen(Node *&a_node, vector<int> &vbt, sTuple &bt, vector<int> &vvt, sTuple &vt, vector<int> &vet, sTuple &et, u32 &maxve, u64 &idx)
	{
		for (int i = 0; i < a_node->m_count; i++)
		{
			int max = 0;
			if (maxve < a_node->m_branch[i].vertex) maxve = a_node->m_branch[i].vertex; 
			if (maxve < a_node->m_branch[i].edge) maxve = a_node->m_branch[i].edge;
			idx++;
			for (int j = 0; j < a_node->m_branch[i].bl->length; j++)
			{
				if (max < a_node->m_branch[i].bl->bt[j].column)
					max = a_node->m_branch[i].bl->bt[j].column;
				if (vbt.size() == BBS)
				{
					bool flag;
					u32 maxValue = 0;
					bt.codeLen += bitOperation::encodingLen(vbt, flag, maxValue);
					bt.codeNumber += vbt.size();
					if (flag) //true
						bt.gamaCounts++;
					else
						bt.fixCounts++;
					vbt.clear();
					if (bt.maxValue < maxValue)
						bt.maxValue = maxValue;
				}
				vbt.push_back(a_node->m_branch[i].bl->bt[j].value);
			}
			bt.bitLen += max + 1;

			max = 0;
			for (int j = 0; j < a_node->m_branch[i].vl->length; j++)
			{
				if (max < a_node->m_branch[i].vl->bt[j].column)
					max = a_node->m_branch[i].vl->bt[j].column;
				if (vvt.size() == VBS)
				{
					bool flag;
					u32 maxValue = 0;
					vt.codeLen += bitOperation::encodingLen(vvt, flag, maxValue);
					vt.codeNumber += vvt.size();
					if (flag) //ture
						vt.gamaCounts++;
					else
						vt.fixCounts++;
					vvt.clear();
					if (vt.maxValue < maxValue)
						vt.maxValue = maxValue;
				}
				vvt.push_back(a_node->m_branch[i].vl->bt[j].value);
			}
			vt.bitLen += max + 1;

			max = 0;
			for (int j = 0; j < a_node->m_branch[i].el->length; j++)
			{
				if (max < a_node->m_branch[i].el->bt[j].column)
					max = a_node->m_branch[i].el->bt[j].column;
				if (vet.size() == EBS)
				{
					bool flag;
					u32 maxValue = 0;
					et.codeLen += bitOperation::encodingLen(vet, flag, maxValue);
					et.codeNumber += vet.size();
					if (flag) //ture
						et.gamaCounts++;
					else
						et.fixCounts++;
					vet.clear();
					if (et.maxValue < maxValue)
						et.maxValue = maxValue;
				}
				vet.push_back(a_node->m_branch[i].el->bt[j].value);
			}
			et.bitLen += max + 1;
		}
	}
	void treeBitEncodingLen(sTuple &bt, sTuple &vt, sTuple &et, u32 &maxve, u64 &idx)
	{
		vector<int> vbt;
		vector<int> vvt;
		vector<int> vet;
		idx = 0;
		maxve = 0;
		this->treeBitGamaLen(this->root, vbt, bt, vvt, vt, vet, et, maxve, idx);

		if (vbt.size() > 0)
		{
			bool flag;
			u32 maxValue = 0;
			bt.codeLen += bitOperation::encodingLen(vbt, flag, maxValue);
			bt.codeNumber += vbt.size();
			if (flag)
				bt.gamaCounts++;
			else
				bt.fixCounts++;
			if (bt.maxValue < maxValue)
				bt.maxValue = maxValue;
		}
		vector<int>().swap(vbt);
		
		if (vvt.size() > 0)
		{
			bool flag;
			u32 maxValue = 0;
			vt.codeLen += bitOperation::encodingLen(vvt, flag, maxValue);
			vt.codeNumber += vvt.size();
			if (flag)
				vt.gamaCounts++;
			else
				vt.fixCounts++;
			if (vt.maxValue < maxValue)
				vt.maxValue = maxValue;
		}
		vector<int>().swap(vvt);

		if (vet.size() > 0)
		{
			bool flag;
			u32 maxValue = 0;
			et.codeLen += bitOperation::encodingLen(vet, flag, maxValue);
			et.codeNumber += vet.size();
			if (flag)
				et.gamaCounts++;
			else
				et.fixCounts++;
			if (et.maxValue < maxValue)
				et.maxValue = maxValue;
		}
		vector<int>().swap(vet);
	}

	void treeBitGamaLen(Node *&a_node, vector<int> &vbt, sTuple &bt, vector<int> &vvt, sTuple &vt, vector<int> &vet, sTuple &et, u32 &maxve, u64 &idx)
	{
		assert(a_node);
		nodeBitGamaLen(a_node, vbt, bt, vvt, vt, vet, et, maxve, idx);
		if (a_node->IsInternalNode())
		{
			for (int i = 0; i < a_node->m_count; i++)
			{
				Node *tmp = (Node *)a_node->m_branch[i].child;
				treeBitGamaLen(tmp, vbt, bt, vvt, vt, vet, et, maxve, idx);
			}
		}
	}
	void staticNode(Node *&tmp, map<int, int> &sd)
	{
		for (int i = 0; i < tmp->m_count; i++)
		{
			BList *bl = tmp->m_branch[i].bl;
			//static the data distribution in bl
			int j = 0;
			while (j < bl->length)
			{
				int count = 1;
				while (bl->bt[j].value == bl->bt[j + count].value)
				{
					count++;
				}
				//
				map<int, int>::iterator iter = sd.find(count); //freq
				if (iter != sd.end())
				{
					iter->second++;
				}
				else
					sd.insert(pair<int, int>(count, 1));
				j += count;
			}
		}
	}
	void staticRunsTree(Node *&a_node, map<int, int> &sd)
	{
		assert(a_node);
		this->staticNode(a_node, sd);
		if (a_node->IsInternalNode())
		{
			for (int i = 0; i < a_node->m_count; i++)
			{
				Node *tmp = (Node *)a_node->m_branch[i].child;
				staticRunsTree(tmp, sd);
			}
		}
		else
			;
	}
	Node *readNode(FILE *&fw)
	{
		Node *newNode = new Node();
		fread(&newNode->m_level, sizeof(int), 1, fw);
		fread(&newNode->m_count, sizeof(int), 1, fw);
		bool f = newNode->m_level == 0;
		int vertex, c, v, length;
		//then read the branch structure
		for (int i = 0; i < newNode->m_count; i++)
		{
			if (f)
			{
				newNode->m_branch[i].child = new int();
				fread(newNode->m_branch[i].child, sizeof(int), 1, fw); // the graph ID in the db
			}
			BList bl; // stack 
			fread(&vertex, sizeof(int), 1, fw);
			fread(&length, sizeof(int), 1, fw);
			for (int j = 0; j < length; j++)
			{
				fread(&c, sizeof(int), 1, fw);
				fread(&v, sizeof(int), 1, fw);
				bl.push_back(BTuple(c, v));
			}
			*newNode->m_branch[i].bl = bl; //deep copy
			newNode->m_branch[i].vertex = vertex;
		}
		return newNode;
	}
	void readBranchTree(Node **a_node, FILE *&fw)
	{
		*a_node = readNode(fw);
		if ((*a_node)->IsInternalNode())
		{
			for (int j = 0; j < (*a_node)->m_count; j++)
			{
				readBranchTree((Node **)&(*a_node)->m_branch[j].child, fw);
			}
		}
	}
	void sequenceNode(Node *&a_node, FILE *&fw)
	{
		fwrite(&a_node->m_level, sizeof(int), 1, fw);
		fwrite(&a_node->m_count, sizeof(int), 1, fw);
		for (int i = 0; i < a_node->m_count; i++)
		{
			//write the branch structure
			//the leaf inforamtion
			if (a_node->m_level == 0) //it has reached to the leaf node 
			{
				int *gid = (int *)a_node->m_branch[i].child;
				fwrite(gid, sizeof(int), 1, fw);
			}
			fwrite(&a_node->m_branch[i].vertex, sizeof(int), 1, fw);
			fwrite(&a_node->m_branch[i].edge, sizeof(int), 1, fw);

			//write the tuple information into disk
			fwrite(&a_node->m_branch[i].bl->length, sizeof(int), 1, fw);
			for (int j = 0; j < a_node->m_branch[i].bl->length; j++)
			{
				fwrite(&a_node->m_branch[i].bl->bt[j].column, sizeof(int), 1, fw);
				fwrite(&a_node->m_branch[i].bl->bt[j].value, sizeof(int), 1, fw);
			}

			//write the label information into disk: (vertex, edge)
			fwrite(&a_node->m_branch[i].vl->length, sizeof(int), 1, fw);
			for (int j = 0; j < a_node->m_branch[i].vl->length; j++)
			{
				fwrite(&a_node->m_branch[i].vl->bt[j].column, sizeof(int), 1, fw);
				fwrite(&a_node->m_branch[i].vl->bt[j].value, sizeof(int), 1, fw);
			}

			//the label information about the edge 
			fwrite(&a_node->m_branch[i].el->length, sizeof(int), 1, fw);
			for (int j = 0; j < a_node->m_branch[i].el->length; j++)
			{
				fwrite(&a_node->m_branch[i].el->bt[j].column, sizeof(int), 1, fw);
				fwrite(&a_node->m_branch[i].el->bt[j].value, sizeof(int), 1, fw);
			}

		}
	}
	void sequenceBranchTree(Node *&a_node, FILE *&fw)
	{
		assert(a_node);
		if (a_node->IsInternalNode())
		{
			sequenceNode(a_node, fw);
			for (int i = 0; i < a_node->m_count; i++)
			{
				Node *tmp = (Node *)a_node->m_branch[i].child;
				sequenceBranchTree(tmp, fw);
			}
			delete a_node; a_node = NULL;
		}
		else
		{
			sequenceNode(a_node, fw);
			delete a_node; a_node = NULL;
		}
	}
	void FreeNode(Node *a_node)
	{

		if (a_node) delete a_node;
		a_node = NULL;
	}
	void RemoveAllRec(Node* a_node)
	{
		if (!a_node) return; //the tree is empty
		if (a_node->IsInternalNode()) // This is an internal node in the tree
		{
			for (int index = 0; index < a_node->m_count; ++index)
			{
				RemoveAllRec((Node *)a_node->m_branch[index].child);
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
	int indexSize(Node *a_node)
	{
		ASSERT(a_node);
		int sum = a_node->nodeMemory();
		if (!a_node->IsInternalNode())
		{
			return sum;
		}
		else
		{
			for (int index = 0; index < a_node->m_count; ++index)
			{
				sum += indexSize((Node *)a_node->m_branch[index].child);
			}
		}
		return sum;
	}

	~branchTree()
	{
		//this->writeIntoDisk();
		RemoveAllRec(root);
		root = NULL;
	}

public:
	void buildBranchTree(string db, int total);
	void buildBranchTree(vector<graph> &vg);
	void writeIntoDisk(const string path = "./bt/branchTree.idx");
	void readFromDisk(const string path = "./bt/branchTree.idx");
	double branchTreeSize();
};
#endif
