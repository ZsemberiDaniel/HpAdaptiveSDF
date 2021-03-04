#pragma once
#include <iostream>

#include "../Structures.h"
#include "octree.h"
#include <queue>

#include "../Math/PolynomialGenerator.h"
#include "../Math/Polynomial.h"

class OctreeGenerator
{
public:
	struct ConstructionParameters
	{
		glm::vec3 minPos = glm::vec3(0);
		float sizeInWorld = 1.0f;
		int maxDegree = 3;
		int maxLevel = 2;
		float errorThreshold = 0.005f;
	};

	template<typename generator, typename sdf>
	static void constructField(Octree<Cell>& octree, generator& polynomialGenerator, const ConstructionParameters& params, sdf sdfFunction);
};

template<typename generator, typename sdf>
void OctreeGenerator::constructField(Octree<Cell>& octree, generator& polynomialGenerator, const ConstructionParameters& params, sdf sdfFunction)
{
	const int initialCellCount = 2;
	float gridCellSize = params.sizeInWorld / initialCellCount;

	auto queueCellCompare = [](const Cell& c1, const Cell& c2) -> bool
	{
		return c1.error < c2.error;
	};

	vector3d<Cell> initialCells(initialCellCount);
	vector3d<Octree<Cell>::Leaf*> generatedCells(initialCellCount);

	float error = 0.0f;
	std::priority_queue< Cell, std::vector<Cell>, decltype(queueCellCompare) > pending(queueCellCompare);

	for (int x = 0; x < initialCellCount; x++)
	{
		for (int y = 0; y < initialCellCount; y++)
		{
			for (int z = 0; z < initialCellCount; z++)
			{
				glm::vec3 cellCoord = params.minPos + gridCellSize * glm::vec3(x, y, z);
				BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + gridCellSize };

				const int startingDegree = 1;
				Polynomial poly = polynomialGenerator.fitPolynomial<sdf>(bbox, startingDegree, sdfFunction);
				float currentError = poly.coefficientBasedError();

				Cell cell = Cell{ bbox, poly, currentError };

				error += currentError;

				initialCells(x, y, z) = cell;
			}
		}
	}
	octree = Octree<Cell>(initialCellCount, initialCells, generatedCells);
	for (int x = 0; x < initialCellCount; x++)
	{
		for (int y = 0; y < initialCellCount; y++)
		{
			for (int z = 0; z < initialCellCount; z++)
			{
				initialCells(x, y, z).octreeLeaf = generatedCells(x, y, z);
				pending.push(initialCells(x, y, z));
			}
		}
	}

	while (!pending.empty() && error > params.errorThreshold)
	{
		auto popped = pending.top();
		pending.pop();

		Cell& currentCell = popped;
		if (currentCell.degree() >= params.maxDegree && currentCell.level() >= params.maxLevel) continue;

		float pImprovement = -std::numeric_limits<float>::max();
		float hImprovement = -std::numeric_limits<float>::max();

		// store the changed variables for p improvement so we don't have to calculate the again to
		// commit these changes to the octree
		Polynomial pImprovementPoly;
		float pImprovementError;

		// same storage as above but for h improvement
		vector3d<Cell> hImpSubdividedCell(2);

		// p-improvement
		// estimate error when using a polynom that is a degree higher
		{
			pImprovementPoly = polynomialGenerator.fitPolynomial<sdf>(currentCell.bbox, currentCell.degree() + 1, sdfFunction);
			pImprovementError = pImprovementPoly.coefficientBasedError();

			// Equation (8), section 3.6
			pImprovement = 1.0f / (Polynomial::calculateCoeffCount(currentCell.degree() + 1) - Polynomial::calculateCoeffCount(currentCell.degree())) *
				(currentCell.error - 8 * pImprovementError);
		}

		// h-improvement
		// subdivide cell into 2x2x2
		{
			// max error for subdivided cells, needed for eq. (9)
			float maxError = -std::numeric_limits<float>::max();
			// how big a subdivided cube's side is
			float subdividedCubeSize = currentCell.bbox.size().x / 2.0f;

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					for (int z = 0; z < 2; z++)
					{
						// the subdivided cubes' coord is shifted from the bigger cube's coord
						glm::vec3 cellCoord = currentCell.bbox.min + glm::vec3(x, y, z) * glm::vec3(subdividedCubeSize);
						BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + glm::vec3(subdividedCubeSize) };

						// polynom for new cell + making new cell
						Polynomial poly = polynomialGenerator.fitPolynomial<sdf>(bbox, currentCell.degree(), sdfFunction);
						float currentError = poly.coefficientBasedError();
						if (maxError < currentError)
						{
							maxError = currentError;
						}

						Cell cell = Cell{ bbox, poly, currentError };

						hImpSubdividedCell(x, y, z) = cell;
					}
				}
			}

			// equation (9), section 3.6
			hImprovement = 1.0f / 7.0f / Polynomial::calculateCoeffCount(currentCell.degree()) * (currentCell.error - 8 * maxError);
		}

		bool refineP = currentCell.degree() < params.maxDegree && (currentCell.level() == params.maxLevel || pImprovement > hImprovement);
		bool refineH = currentCell.level() < params.maxLevel && !refineP;

		// deciding in favor of p-improvement
		if (refineP/*pImprovement >= hImprovement*/)
		{
			error += pImprovementError - currentCell.error;

			currentCell.poly = pImprovementPoly;
			currentCell.error = pImprovementError;
			// printPolynomial(currentCell.poly);

			currentCell.octreeLeaf->setValue(currentCell);

			pending.push(currentCell);
		}
		// doing h-improvement otherwise
		if (refineH/*pImprovement < hImprovement*/)
		{
			error -= currentCell.error;

			vector3d<Octree<Cell>::Leaf*> leaves(2);
			currentCell.octreeLeaf->subdivide(hImpSubdividedCell, leaves);

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					for (int z = 0; z < 2; z++)
					{
						hImpSubdividedCell(x, y, z).octreeLeaf = leaves(x, y, z);
						pending.push(hImpSubdividedCell(x, y, z));

						error += hImpSubdividedCell(x, y, z).error;
					}
				}
			}
		}
	}
}
