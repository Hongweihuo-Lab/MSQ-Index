#ifndef _BRANCH_H
#define _BRANCH_H
#include "stdafx.h"
#include "BList.h"

class branch  
{
public:
	BList *bl;
	int vertex;
	int edge;
	void *child; //it is Node or graphId information
	BList *vl; 
	BList *el;

public:

	branch()
	{
		child = NULL;
		vertex = 0;
		edge = 0;
		this->bl = new BList();
		this->vl = new BList();
		this->el = new BList();

	}
	branch(map<int, int> &vlabel, map<int, int> &elabel, int v, int e)
	{
		vl = new BList(vlabel);
		el = new BList(elabel);
		child = NULL;
		bl = NULL;
		vertex = v;
		edge = e;
	}

	branch(map<int, int> &feature, map<int, int> &vlabel, map<int, int> &elabel, int v, int e)
	{
		bl = new BList(feature);
		vl = new BList(vlabel);
		el = new BList(elabel);
		child = NULL;
		vertex = v;
		edge = e;
	}

	//light copy
	branch(const branch &bh)
	{
		this->bl = bh.bl;
		this->el = bh.el;
		this->vl = bh.vl;
		this->vertex = bh.vertex;
		this->edge = bh.edge;
		this->child = bh.child;
	}
	
	branch& operator=(const branch &bh)
	{
		if (this == &bh)
			return *this;
		*this->bl = *bh.bl; //deep copy
		*this->el = *bh.el; 
		*this->vl = *bh.vl;
		this->vertex = bh.vertex;
		this->edge = bh.edge;
		this->child = bh.child;
		return *this;
	}

	BList *mergeList(BList *b1, BList *b2)
	{
		BList *bl = new BList();
		int i = 0, j = 0;
		while (i < b1->length && j < b2->length)
		{
			if (b1->bt[i] == b2->bt[j])
			{
				BTuple temp;
				temp.column = b1->bt[i].column;
				temp.value = max(b1->bt[i].value, b2->bt[j].value);
				bl->push_back(temp);
				i++;
				j++;
			}
			else if (b1->bt[i] < b2->bt[j])
			{
				bl->push_back(b1->bt[i]);
				i++;
			}
			else
			{
				bl->push_back(b2->bt[j]);
				j++;
			}
		}
		while (i < b1->length)
		{
			bl->push_back(b1->bt[i]);
			i++;
		}
		while (j < b2->length)
		{
			bl->push_back(b2->bt[j]);
			j++;
		}
		return bl;
	}

	int unionListSum(BList *b1, BTuple bt[], int &count)
	{
		int sum = 0;
		int i = 0, j = 0;
		while (i < b1->length && j < count)
		{
			if (b1->bt[i] == bt[j])
			{
				sum += min(b1->bt[i].value, bt[j].value);
				i++;
				j++;
			}
			else if (b1->bt[i] < bt[j])
				i++;
			else
				j++;
		}
		return sum;
	}
	
	int unionListSum(BList *b1, BList *b2)
	{		
		int sum = 0;
		int i = 0, j = 0;
		while (i < b1->length && j < b2->length)
		{
			if (b1->bt[i] == b2->bt[j])
			{
				sum += min(b1->bt[i].value, b2->bt[j].value);
				i++;
				j++;
			}
			else if (b1->bt[i] < b2->bt[j])
				i++;
			else
				j++;
		}
		return sum;
	}
	~branch()
	{
		if (this->bl) delete bl;
		if (this->el) delete el;
		if (this->vl) delete vl;
	}
	void clear()
	{
		if (this->bl) delete bl; {bl = new BList(); }
		if (this->el) delete el; {el = new BList(); }
		if (this->vl) delete vl; {vl = new BList(); }
 	}
	u64 normalSize()
	{
		u64 sum = 0;
		sum += this->bl->BListSize();
		sum += this->el->BListSize();
		sum += this->vl->BListSize();
		sum += sizeof(bl);
		sum += sizeof(edge);
		sum += sizeof(el);
		sum += sizeof(vl);
		sum += sizeof(vertex);
		sum += sizeof(child);
		return sum;
	}
	u64 vlSize()
	{
		return this->vl->BListSize();
	}
	u64 elSize()
	{
		return this->el->BListSize();
	}
	u64 blSize()
	{
		return this->bl->BListSize();
	}

};
#endif