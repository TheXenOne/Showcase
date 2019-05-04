#pragma once

#include <AI/Pathfinding/GraphEdge.h>
#include <AI/Pathfinding/NodeNavigation.h>
#include <AI/Pathfinding/SparseGraph.h>
#include <AI/Pathfinding/Graph_SearchDijkstra.h>
#include <AI/Pathfinding/NodeTypeEnumerations.h>

#include <ctime>
#include <vector>
#include <iostream>

template <class graph_type>
class Graph_FlowField
{
public:
	typedef typename graph_type::EdgeType Edge;
	typedef std::vector<const Edge*> ShortestPathTree;

	Graph_FlowField(const graph_type & graph)
		: m_Graph(graph)
		, m_FlowFields(graph.NumNodes(), std::vector<const Edge*>(graph.NumNodes(), nullptr))
	{}

	void	GenerateAllFlowFields(); // Generates all flow fields for the given graph
	int		GetNextNodeInPathToTarget(int a_target, int a_closestNode, bool a_generatePathsIfNotPresent = false);
	void	GenerateFlowFieldForNode(int node);
private:

	std::vector<ShortestPathTree>		m_FlowFields; //Indexed into by target node then closest node
	const graph_type&					m_Graph;
};

template<class graph_type>
void Graph_FlowField<graph_type>::GenerateAllFlowFields()
{
	for (int i = 0; i < m_Graph.NumNodes(); ++i)
	{
		GenerateFlowFieldForNode(i);
	}
}

template<class graph_type>
int Graph_FlowField<graph_type>::GetNextNodeInPathToTarget(int a_target, int a_closestNode, bool a_generatePathsIfNotPresent)
{
	if (!m_FlowFields[a_target][a_closestNode])
	{
		if (!a_generatePathsIfNotPresent)
		{
			return invalid_node_index;
		}
		else
		{
			GenerateFlowFieldForNode(a_target);
		}
	}

	return m_FlowFields[a_target][a_closestNode]->From();
}

template<class graph_type>
void Graph_FlowField<graph_type>::GenerateFlowFieldForNode(int node)
{
	std::clock_t start;
	start = std::clock();

	Graph_SearchDijkstra<SparseGraph<NodeNavigation, GraphEdge>> graphSearch(m_Graph, node);

	double dur = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "Flow field for 1 node generated in " << dur << std::endl;

	m_FlowFields[node] = graphSearch.GetAllPaths();
}
