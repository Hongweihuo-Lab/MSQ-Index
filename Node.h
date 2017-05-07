#ifndef _NODE_H
#define _NODE_H
#include "branch.h"

struct Node
{
	bool IsInternalNode()                         { return (m_level > 0); } 
	bool IsLeaf()									  { return (m_level == 0); }
	int m_count;                                
	int m_level;                                 
	branch *m_branch;  
	Node()
	{
		m_count = 0;
		m_level = 0;
		m_branch = new branch[MAXNODES];
	}
	uint nodeMemory()
	{
		uint sum = 0;
		sum += sizeof(m_count);
		sum += sizeof(m_level);
		for (int i = 0; i < m_count; i++)
			sum += m_branch[i].normalSize();
		
		return sum;
	}
	~Node()
	{
		if (m_branch) delete[] m_branch;
	}
};

#endif