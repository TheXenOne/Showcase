#pragma once

#include <DirectXMath.h>
#include <cmath>

class Heuristic_Euclidean
{
public:
	template <class graph_type>
	static float Calculate(const graph_type& a_graph, const int& a_node1, const int& a_node2) //Returns straight line distance between two nodes
	{
		DirectX::XMFLOAT3 distance;
		DirectX::XMStoreFloat3(&distance, DirectX::XMVector3Length(DirectX::XMVectorSubtract((a_graph.GetNode(a_node1).GetPosition(), a_graph.GetNode(a_node2).GetPosition()))));
		return distance.x;
	}
private:
	Heuristic_Euclidean() {}
};

class Heuristic_Manhatten
{
public:
	template <class graph_type>
	static float Calculate(const graph_type& a_graph, const int& a_targetNode, const int& a_currentNode) //Returns grid distance between two nodes
	{
		return (fabs((DirectX::XMVectorGetX(a_graph.GetNode(a_targetNode).GetPosition()) - DirectX::XMVectorGetX(a_graph.GetNode(a_currentNode).GetPosition()))) +
			fabs(DirectX::XMVectorGetZ(a_graph.GetNode(a_targetNode).GetPosition()) - DirectX::XMVectorGetZ(a_graph.GetNode(a_currentNode).GetPosition())));
	}
private:
	Heuristic_Manhatten() {}
};