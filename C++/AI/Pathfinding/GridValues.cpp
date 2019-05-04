#include <AI/Pathfinding/GridValues.h>

#include <list>

bool GridChecker::CheckIsNeighbourOfNode(int a_graphNumNodes, int a_possibleNeighbourNode, int a_nodeToCheckAgainst, int a_graphNumCellsWidth)
{
	std::list<int> neighbours = std::list<int>();
	neighbours.push_back(a_nodeToCheckAgainst - a_graphNumCellsWidth - 1);
	neighbours.push_back(a_nodeToCheckAgainst - a_graphNumCellsWidth);
	neighbours.push_back(a_nodeToCheckAgainst - a_graphNumCellsWidth + 1);
	neighbours.push_back(a_nodeToCheckAgainst - 1);
	neighbours.push_back(a_nodeToCheckAgainst + 1);
	neighbours.push_back(a_nodeToCheckAgainst + a_graphNumCellsWidth - 1);
	neighbours.push_back(a_nodeToCheckAgainst + a_graphNumCellsWidth);
	neighbours.push_back(a_nodeToCheckAgainst + a_graphNumCellsWidth + 1);

	for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
	{
		if (a_possibleNeighbourNode == *it && *it < a_graphNumNodes)
		{
			return true;
		}
	}

	return false;
}
