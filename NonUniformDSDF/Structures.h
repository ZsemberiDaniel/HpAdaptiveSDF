#pragma once
#include "Polynomial.h"

struct Grid
{
	glm::vec3 minPos;
	// TODO: underneath
	// needs to be foxed to 2
	int initialCellCount;
	// needs to be a float
	glm::vec3 cellSize;
};

struct BoundingBox
{
	glm::vec3 min = glm::vec3();
	glm::vec3 max = glm::vec3();

	glm::vec3 size() const
	{
		return max - min;
	}
};

struct Cell
{
	BoundingBox bbox = BoundingBox{};
	Polynomial poly;
	float error = 0.0f;
	int degree = -1;
	int level = -1;
	Octree<Cell>::Leaf* octreeLeaf = nullptr;


	bool operator< (const Cell& cell) const
	{
		return false;
	}
};
