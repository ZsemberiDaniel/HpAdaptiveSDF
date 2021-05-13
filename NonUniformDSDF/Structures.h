#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "Math/Polynomial.h"
#include "Octree/octree.h"

struct iBoundingBox
{
	glm::ivec3 min = glm::ivec3();
	glm::ivec3 max = glm::ivec3();

	glm::ivec3 size() const
	{
		return max - min;
	}
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
	// bbox in coordinates
	BoundingBox bbox = BoundingBox{};
	// stored polynomial
	Polynomial poly;
	// stored polynomial error (for caching purposes, it can be calculated from poly as well)
	float error = 0.0f;
	// stored octreeLeaf
	std::shared_ptr<Octree<Cell>::Leaf> octreeLeaf = nullptr;

	int level() const
	{
		return octreeLeaf->layer();
	}

	int degree() const
	{
		return poly.getDegree();
	}
};
#endif