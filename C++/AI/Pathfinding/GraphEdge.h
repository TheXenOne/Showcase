#pragma once

#include <ostream>

class GraphEdge
{
public:
	GraphEdge();
	GraphEdge(int from, int to);
	GraphEdge(int from, int to, float cost);
	virtual ~GraphEdge();

	void Clear();

	void SetFromTo(int a_from, int a_to);
	void SetFromToCost(int a_from, int a_to, float a_cost);

	int From() const;
	void SetFrom(int a_newIndex);

	int To() const;
	void SetTo(int a_newIndex);

	float Cost() const;
	void SetCost(float a_newCost);

	bool operator==(const GraphEdge& a_rhs);
	bool operator!=(const GraphEdge& a_rhs);

	friend std::ostream& operator<<(std::ostream& a_os, const GraphEdge& a_edge)
	{
		a_os << "From: " << a_edge.m_From << " To: " << a_edge.m_To << " Cost: " << a_edge.m_Cost << std::endl;
		return a_os;
	}
protected:
	int m_From;
	int m_To;
	float m_Cost;
};