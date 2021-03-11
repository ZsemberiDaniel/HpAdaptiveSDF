#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "Math/Polynomial.h"
#include "Octree/octree.h"

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
	std::shared_ptr<Octree<Cell>::Leaf> octreeLeaf = nullptr;

	int level() const
	{
		return octreeLeaf->layer();
	}

	int degree() const
	{
		return poly.getDegree();
	}

	bool operator< (const Cell& cell) const
	{
		return false;
	}
};
#endif