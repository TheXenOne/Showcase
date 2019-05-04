#pragma once

#include <ostream>

class GraphNode
{
public:
	GraphNode();
	GraphNode(int index);
	virtual ~GraphNode();

	int Index() const;
	void SetIndex(int a_newIndex);
	void RevertIndex();

	virtual void SaveNode(std::ostream& a_os) const;

protected:
	int m_Index;
	int m_PreviousIndex;
};