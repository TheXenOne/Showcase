#pragma once

#include <DirectXMath.h>

#include <AI/Pathfinding/GridValues.h>

#include <vector>

template <class graph_type, class node_type, class edge_type>
class GraphGenerator
{
public:
	typedef edge_type EdgeType;
	typedef node_type NodeType;

	static GridValues* GenerateGrid(graph_type * a_graph, float a_mapWidth, float a_mapHeight, float a_cellResolutionWidth, float a_cellResolutionHeight, bool a_diagonalMovementAllowed = true);
private:
	GraphGenerator() {}
};

template<class graph_type, class node_type, class edge_type>
GridValues* GraphGenerator<graph_type, node_type, edge_type>::GenerateGrid(graph_type * a_graph, float a_mapWidth, float a_mapHeight, float a_cellResolutionWidth, float a_cellResolutionHeight, bool a_diagonalMovementAllowed)
{
	GridValues * gridProperties = new GridValues();
	gridProperties->mapWidth = a_mapWidth;
	gridProperties->mapHeight = a_mapHeight;
	gridProperties->cellResolutionWidth = a_cellResolutionWidth;
	gridProperties->cellResolutionHeight = a_cellResolutionHeight;
	gridProperties->diagonalMovementAllowed = a_diagonalMovementAllowed;
	gridProperties->numCellsWidth = (int)(a_mapWidth / a_cellResolutionWidth);
	gridProperties->numCellsHeight = (int)(a_mapHeight / a_cellResolutionHeight);

	a_graph->Clear();
	a_graph->SetDigraph(true);

	int numCellsWidth = (int)(a_mapWidth / a_cellResolutionWidth);
	int numCellsHeight = (int)(a_mapHeight / a_cellResolutionHeight);

	int nextNodeIndex = 0;

	float halfCellWidth = a_cellResolutionWidth / 2.f;
	float halfCellHeight = a_cellResolutionHeight / 2.f;

	float cellX = 0.f + halfCellWidth;
	float cellZ = a_mapHeight - halfCellHeight;

	DirectX::XMFLOAT3 pos;
	pos.y = 0.f;

	NodeType node(0);
	EdgeType edgeLeft;
	EdgeType prevEdgeRight;
	EdgeType edgeTop;
	EdgeType prevEdgeBottom;
	EdgeType edgeTopLeft;
	EdgeType prevEdgeBottomRight;
	EdgeType edgeTopRight;
	EdgeType prevEdgeBottomLeft;

	for (int i = 0; i < numCellsHeight; ++i)
	{
		for (int j = 0; j < numCellsWidth; ++j)
		{
			//Node index
			node.SetIndex(nextNodeIndex); //Set index of node to next available index
			++nextNodeIndex; //Increase index for next node
			
			//Node position
			pos.x = cellX; //Set x position of node to new cell X
			pos.z = cellZ; //Set y position of node to new cell Y
			node.SetPosition(pos); //Set the position of the node
			cellX += a_cellResolutionWidth; //Increase the x position of the cell by the width for each cell

			a_graph->AddNode(node); //Add the node to the graph

			//Clear edges
			edgeLeft.Clear();
			prevEdgeRight.Clear();
			edgeTop.Clear();
			prevEdgeBottom.Clear();
			edgeTopLeft.Clear();
			prevEdgeBottomRight.Clear();
			edgeTopRight.Clear();
			prevEdgeBottomLeft.Clear();

			if (j != 0) //If not in the first column, add edge to the left (and corresponding reverse edge)
			{
				edgeLeft.SetFromToCost(node.Index(), (node.Index() - 1), 1.f);
				a_graph->AddEdge(edgeLeft);
				prevEdgeRight.SetFromToCost((node.Index() - 1), node.Index(), 1.f);
				a_graph->AddEdge(prevEdgeRight);
			}
			if (i != 0) //If not in the first row, add edge to the top (and corresponding reverse edge)
			{
				edgeTop.SetFromToCost(node.Index(), (node.Index() - numCellsWidth), 1.f);
				a_graph->AddEdge(edgeTop);
				prevEdgeBottom.SetFromToCost((node.Index() - numCellsWidth), node.Index(), 1.f);
				a_graph->AddEdge(prevEdgeBottom);
			}
			if (a_diagonalMovementAllowed) //If movement between diagonally adjacent cells is allowed
			{
				if ((j != 0) && (i != 0)) //If not in first column & row, add edge to bottom left (and corresponding reverse edge)
				{
					edgeTopLeft.SetFromToCost(node.Index(), (node.Index() - numCellsWidth - 1), 1.f);
					a_graph->AddEdge(edgeTopLeft);
					prevEdgeBottomRight.SetFromToCost((node.Index() - numCellsWidth - 1), node.Index(), 1.f);
					a_graph->AddEdge(prevEdgeBottomRight);
				}
				if ((i != 0) && (j != numCellsWidth - 1)) //If not in the first row && not in the last column, add an edge going 
				{
					edgeTopRight.SetFromToCost(node.Index(), node.Index() - numCellsWidth + 1, 1.f);
					a_graph->AddEdge(edgeTopRight);
					prevEdgeBottomLeft.SetFromToCost(node.Index() - numCellsWidth + 1, node.Index(), 1.f);
					a_graph->AddEdge(prevEdgeBottomLeft);
				}
			}
		}
		cellX = 0.f + halfCellWidth; //Reset the cell x position, as we are on a new row
		cellZ -= a_cellResolutionHeight; //Decrease the cell y position by the height for each cell
	}

	return gridProperties;
}
