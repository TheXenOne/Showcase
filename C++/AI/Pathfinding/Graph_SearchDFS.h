#pragma once

#include "GraphNode.h"
#include "GraphEdge.h"
#include "SparseGraph.h"

#include <list>
#include <stack>

template <class graph_type>
class Graph_SearchDFS
{
public:
	Graph_SearchDFS(const graph_type& graph, int startNode, int target = -1) : m_Graph(graph),
		m_StartNode(startNode),
		m_TargetNode(target),
		m_PathFound(false),
		m_Visited(m_Graph.NumNodes(), unvisited),
		m_NodeParents(m_Graph.NumNodes(), no_parent_assigned)
	{
		m_PathFound = Search();
	}

	bool IsPathFound() const { return m_PathFound; }
	std::list<int> GetPathToTarget() const;
private:
	Graph_SearchDFS();
	bool Search();

	enum
	{
		visited,
		unvisited,
		no_parent_assigned
	};

	typedef typename graph_type::EdgeType Edge;
	typedef typename graph_type::NodeType Node;

	const graph_type& m_Graph; //Reference to graph to be searched
	std::vector<int> m_Visited; //Records all nodes visited
	std::vector<int> m_NodeParents; //Vector of parent nodes for a given node index
	int m_StartNode;
	int m_TargetNode;
	bool m_PathFound;
};

template<class graph_type>
std::list<int> Graph_SearchDFS<graph_type>::GetPathToTarget() const
{
	std::list<int> path;

	if (!m_PathFound || m_TargetNode < 0)
	{
		return path;
	}

	int node = m_TargetNode;

	path.push_back(node);

	while (node != m_StartNode)
	{
		node = m_NodeParents[node];

		path.push_back(node);
	}

	return path;
}

template<class graph_type>
bool Graph_SearchDFS<graph_type>::Search()
{
	std::stack<const Edge*> stack;

	//Create first dummy edge so the stack is not empty
	Edge firstEdgeDummy(m_StartNode, m_StartNode, 0.f);
	stack.push(&firstEdgeDummy);

	while (!stack.empty())
	{
		const Edge* nextEdge = stack.top();

		stack.pop(); //Remove topmost edge from the stack

		m_NodeParents[nextEdge->To()] = nextEdge->From(); //Note parent of destination node

		m_Visited[nextEdge->To()] = visited; //Mark destination node as visited

		if (nextEdge->To() == m_TargetNode) //Test for success
		{
			return true;
		}

		graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, nextEdge->To());

		for (const Edge* e = ConstEdgeItr.begin(); !ConstEdgeItr.end(); e = ConstEdgeItr.next()) //Loop through adjacent edges
		{
			if (m_Visited[e->To()] == unvisited) //If unvisited, push onto the stack
			{
				stack.push(e);
			}
		}
	}

	return false; //No path found
}