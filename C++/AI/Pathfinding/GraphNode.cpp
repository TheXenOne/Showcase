#include <AI/Pathfinding/GraphNode.h>
#include <AI/Pathfinding/NodeTypeEnumerations.h>

GraphNode::GraphNode() : m_Index(invalid_node_index)
{
}

GraphNode::GraphNode(int index) : m_Index(index), m_PreviousIndex(invalid_node_index)
{
}

GraphNode::~GraphNode()
{
}

int GraphNode::Index() const
{
	return m_Index;
}

void GraphNode::SetIndex(int a_newIndex)
{
	m_PreviousIndex = m_Index;
	m_Index = a_newIndex;
}

void GraphNode::RevertIndex()
{
	int temp = m_Index;
	m_Index = m_PreviousIndex;
	m_PreviousIndex = temp;
}

void GraphNode::SaveNode(std::ostream & a_os) const
{
	a_os << "Index: " << m_Index << std::endl;
}
