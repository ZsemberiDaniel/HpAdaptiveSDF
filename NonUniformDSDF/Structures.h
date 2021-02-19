#pragma once

struct Grid
{
	glm::vec3 minPos;
	int initialCellCount;
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

struct Polynomial
{
	std::vector<float> coeffs;
	int degree;
};

struct Cell
{
	BoundingBox bbox = BoundingBox{};
	Polynomial poly = Polynomial{};
	float error = 0.0f;
	int degree = -1;
	int level = -1;
	Octree<Cell>::Leaf* octreeLeaf = nullptr;


	bool operator< (const Cell& cell) const
	{
		return false;
	}
};

struct GaussCoeffPoints
{
	std::vector<float> coeffs;
	std::vector<glm::vec3> points;
};
