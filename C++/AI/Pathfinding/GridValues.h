#pragma once

struct GridValues
{
	float mapWidth;
	float mapHeight;
	float cellResolutionWidth;
	float cellResolutionHeight;
	int numCellsWidth;
	int numCellsHeight;
	bool diagonalMovementAllowed;
};

class GridChecker
{
public:
	static bool CheckIsNeighbourOfNode(int a_graphNumNodes, int a_possibleNeighbourNode, int a_nodeToCheckAgainst, int a_graphNumCellsWidth = 60);
private:
	GridChecker() {}
};