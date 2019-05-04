#pragma once

#include "GraphEdge.h"
#include "Heuristics.h"
#include "NodeNavigation.h"
#include "PriorityQueue.h"
#include "SparseGraph.h"

#include <ctime>
#include <iostream>
#include <vector>
#include <list>

template <class graph_type, class heuristic>
class Graph_SearchAStar
{
private:
	typedef typename graph_type::EdgeType Edge;
	typedef typename graph_type::NodeType Node;

	enum
	{
		visited,
		unvisited,
		no_parent_assigned
	};

	const graph_type& m_Graph; //Reference to graph to be searched
	std::vector<const Edge*> m_ShortestPathTree; //Edges that comprise best paths from every node on SPT to Start Node
	std::vector<float> m_CostToNode; //Total cost to node (accessed via node index)
	std::vector<float> m_EstimatedCostToTargetFromNode; //Cost to node + heuristic, estimating the total cost to the destination from a node (accessed by node index)
	std::vector<const Edge*> m_NodeParents; //Vector of parent edges leading to nodes in SPT, that aren't in the SPT yet (accessed by node index)
	int m_StartNode;
	int m_TargetNode;
	int m_NodesSearched;
public:
	Graph_SearchAStar(const graph_type& graph, int startNode, int target = -1) : m_Graph(graph),
		m_ShortestPathTree(graph.NumNodes()),
		m_NodeParents(graph.NumNodes()),
		m_CostToNode(graph.NumNodes(), 0.f),
		m_EstimatedCostToTargetFromNode(graph.NumNodes(), 0.f),
		m_StartNode(startNode),
		m_TargetNode(target),
		m_NodesSearched(0)
	{
		Search();
	}

	std::vector<const Edge*> GetAllPaths() const { return m_ShortestPathTree; } //Returns SPT for either whole graph, or until target is found
	std::list<int> GetPathToTarget() const; //Returns path by working through SPT backwards from target
	float GetCostToTarget() const; //Returns total cost to target
	int GetNodesSearched() const { return m_NodesSearched; }
private:
	Graph_SearchAStar() {}
	void Search();
};

template <class graph_type, class heuristic>
std::list<int> Graph_SearchAStar<graph_type, heuristic>::GetPathToTarget() const
{
	std::list<int> path;

	//just return an empty path if no target or no path found
	if (m_TargetNode < 0)
	{
		return path;
	}

	int node = m_TargetNode;

	path.push_front(node);

	while ((node != m_StartNode) && (m_ShortestPathTree[node] != 0))
	{
		node = m_ShortestPathTree[node]->From();

		path.push_front(node);
	}

	return path;
}

template <class graph_type, class heuristic>
float Graph_SearchAStar<graph_type, heuristic>::GetCostToTarget() const
{
	return m_CostToNode[m_TargetNode];
}

template <class graph_type, class heuristic>
void Graph_SearchAStar<graph_type, heuristic>::Search()
{
	IndexedPriorityQLow<float> priorityQueue(m_EstimatedCostToTargetFromNode, m_Graph.NumNodes()); //Indexed priority queue, with lowest estimated cost node first

	priorityQueue.insert(m_StartNode); //Add start node

	while (!priorityQueue.empty())
	{
		++m_NodesSearched;

		int nextClosestNode = priorityQueue.Pop(); //Take lowest cost node from frontier

		m_ShortestPathTree[nextClosestNode] = m_NodeParents[nextClosestNode]; //Move edge from parents to shortest path tree

		if (nextClosestNode == m_TargetNode) //Check for success
		{
			return;
		}

		graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, nextClosestNode);

		for (const Edge* edge = ConstEdgeItr.begin(); !ConstEdgeItr.end(); edge = ConstEdgeItr.next()) //Loop through all edges adjacent to nextClosestNode
		{
			if (m_NodeParents[edge->To()] == 0) //If the node has no parent yet (i.e. it hasn't been on the frontier yet)
			{
				float heuristicCost = heuristic::Calculate(m_Graph, m_TargetNode, edge->To()); //Calculate heuristic cost for this edge

				float nextNodeCost = m_CostToNode[nextClosestNode] + edge->Cost(); //Note the cost to get to the node this edge leads to

				m_EstimatedCostToTargetFromNode[edge->To()] = nextNodeCost + heuristicCost; //Set estimated cost to destination using this path

				m_CostToNode[edge->To()] = nextNodeCost; //Set the cost to this node

				priorityQueue.insert(edge->To()); //Add it to the priority queue

				m_NodeParents[edge->To()] = edge; //Set its parent to the current edge
			}
			else if ((m_CostToNode[nextClosestNode] + edge->Cost() < m_CostToNode[edge->To()]) && m_ShortestPathTree[edge->To()] == 0) //If the cost using current node is < existing path
			{
				float heuristicCost = heuristic::Calculate(m_Graph, m_TargetNode, edge->To()); //Calculate heuristic cost for this edge

				float nextNodeCost = m_CostToNode[nextClosestNode] + edge->Cost(); //Note the cost to get to the node this edge leads to

				m_EstimatedCostToTargetFromNode[edge->To()] = nextNodeCost + heuristicCost;//Set estimated cost to destination using this path

				m_CostToNode[edge->To()] = nextNodeCost; //Set new cost of node

				priorityQueue.ChangePriority(edge->To()); //Change its priority in the queue to match new cost

				m_NodeParents[edge->To()] = edge; //Set its parent to the current edge
			}
		}
	}
}
