#include <AI/Pathfinding/GraphEdge.h>
#include <AI/Pathfinding/NodeTypeEnumerations.h>

GraphEdge::GraphEdge() : m_From(invalid_node_index), m_To(invalid_node_index), m_Cost(1.f)
{
}

GraphEdge::GraphEdge(int from, int to) : m_From(from), m_To(to), m_Cost(1.f)
{
}

GraphEdge::GraphEdge(int from, int to, float cost) : m_From(from), m_To(to), m_Cost(cost)
{
}

GraphEdge::~GraphEdge()
{
}

void GraphEdge::Clear()
{
	m_From = invalid_node_index;
	m_To = invalid_node_index;
	m_Cost = 1.f;
}

void GraphEdge::SetFromTo(int a_from, int a_to)
{
	m_From = a_from;
	m_To = a_to;
}

void GraphEdge::SetFromToCost(int a_from, int a_to, float a_cost)
{
	m_From = a_from;
	m_To = a_to;
	m_Cost = a_cost;
}

int GraphEdge::From() const
{
	return m_From;
}

void GraphEdge::SetFrom(int a_newIndex)
{
	m_From = a_newIndex;
}

int GraphEdge::To() const
{
	return m_To;
}

void GraphEdge::SetTo(int a_newIndex)
{
	m_To = a_newIndex;
}

float GraphEdge::Cost() const
{
	return m_Cost;
}

void GraphEdge::SetCost(float a_newCost)
{
	m_Cost = a_newCost;
}

bool GraphEdge::operator==(const GraphEdge & a_rhs)
{
	return a_rhs.m_From == this->m_From &&
		a_rhs.m_To == this->m_To &&
		a_rhs.m_Cost == this->m_Cost;
}

bool GraphEdge::operator!=(const GraphEdge & a_rhs)
{
	return !(*this == a_rhs);
}
