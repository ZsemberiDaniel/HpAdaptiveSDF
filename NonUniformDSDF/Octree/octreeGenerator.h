#ifndef OCTREEGENERATOR_H
#define OCTREEGENERATOR_H
#include <iostream>

#include "../Structures.h"
#include "octree.h"

#include "../Math/Polynomial.h"
#include "../constants.h"

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
		bool useHAdapt = true;
		bool usePAdapt = true;
	};

	template<typename generator, typename sdf>
	static void constructField(std::unique_ptr<Octree<Cell>>& octree, generator& polynomialGenerator, const ConstructionParameters& params, sdf& sdfFunction);
};

template<typename generator, typename sdf>
void OctreeGenerator::constructField(std::unique_ptr<Octree<Cell>>& octree, generator& polynomialGenerator, const ConstructionParameters& params, sdf& sdfFunction)
{
	if (!params.useHAdapt && !params.usePAdapt)
	{
		octree = nullptr;
		std::cout << "Cannot construct octree without either h and p adapt! (Both set to false in constructPatams)" << std::endl;
		return;
	}

	float gridCellSize = params.sizeInWorld / INITIAL_CELL_COUNT;

	auto queueCellCompare = [](const Cell& c1, const Cell& c2) -> bool
	{
		return c1.error < c2.error;
	};

	vector3d<Cell> initialCells(INITIAL_CELL_COUNT);
	vector3d<std::shared_ptr<Octree<Cell>::Leaf>> generatedCells(INITIAL_CELL_COUNT);

	float error = 0.0f;
	// std::priority_queue< Cell, std::vector<Cell>, decltype(queueCellCompare) > pending(queueCellCompare);
	std::vector<Cell> pendingVector;

	for (int x = 0; x < INITIAL_CELL_COUNT; x++)
	{
		for (int y = 0; y < INITIAL_CELL_COUNT; y++)
		{
			for (int z = 0; z < INITIAL_CELL_COUNT; z++)
			{
				glm::vec3 cellCoord = params.minPos + gridCellSize * glm::vec3(x, y, z);
				BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + gridCellSize };

				const int startingDegree = 2;
				Polynomial poly = polynomialGenerator.fitPolynomial<sdf>(bbox, startingDegree, sdfFunction);
				float currentError = poly.coefficientBasedError();

				Cell cell = Cell{ bbox, poly, currentError };

				error += currentError;

				initialCells(x, y, z) = cell;
			}
		}
	}
	octree = std::make_unique<Octree<Cell>>(INITIAL_CELL_COUNT, initialCells, generatedCells);
	for (int x = 0; x < INITIAL_CELL_COUNT; x++)
	{
		for (int y = 0; y < INITIAL_CELL_COUNT; y++)
		{
			for (int z = 0; z < INITIAL_CELL_COUNT; z++)
			{
				initialCells(x, y, z).octreeLeaf = generatedCells(x, y, z);
				
				pendingVector.push_back(initialCells(x, y, z));
				std::push_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);
				// pending.push(initialCells(x, y, z));
			}
		}
	}

	int step = 0;
	while (!pendingVector.empty() && error > params.errorThreshold)
	{
		/*auto popped = pending.top();
		pending.pop();*/
		std::pop_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);
		auto popped = pendingVector.back();
		pendingVector.pop_back();

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
		if (params.usePAdapt) {
			pImprovementPoly = polynomialGenerator.fitPolynomial<sdf>(currentCell.bbox, currentCell.degree() + 1, sdfFunction);
			pImprovementError = pImprovementPoly.coefficientBasedError();

			// Equation (8), section 3.6
			pImprovement = 1.0f / (Polynomial::calculateCoeffCount(currentCell.degree() + 1) - Polynomial::calculateCoeffCount(currentCell.degree())) *
				(currentCell.error - 8 * pImprovementError);
		}

		// h-improvement
		// subdivide cell into 2x2x2
		if (params.useHAdapt) {
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

		// both false not possible (checked at beginning of function)
		bool refineP, refineH;
		if (params.useHAdapt && !params.usePAdapt)
		{
			refineH = true;
			refineP = false;
		}
		else if (params.usePAdapt && !params.useHAdapt)
		{
			refineP = true;
			refineH = false;
		}
		else
		{
			refineP = currentCell.degree() < params.maxDegree && (currentCell.level() == params.maxLevel || pImprovement > hImprovement);
			refineH = currentCell.level() < params.maxLevel && !refineP;
		}

		// deciding in favor of p-improvement
		if (refineP/*pImprovement >= hImprovement*/)
		{
			error += pImprovementError - currentCell.error;

			currentCell.poly = pImprovementPoly;
			currentCell.error = pImprovementError;
			// printPolynomial(currentCell.poly);

			currentCell.octreeLeaf->setValue(currentCell);

			// pending.push(currentCell);
			pendingVector.push_back(currentCell);
			std::push_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);
		}
		// doing h-improvement otherwise
		if (refineH/*pImprovement < hImprovement*/)
		{
			error -= currentCell.error;

			vector3d<std::shared_ptr<Octree<Cell>::Leaf>> leaves(2);
			currentCell.octreeLeaf->subdivide(octree.get(), hImpSubdividedCell, leaves);

			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					for (int z = 0; z < 2; z++)
					{
						hImpSubdividedCell(x, y, z).octreeLeaf = leaves(x, y, z);
						
						// pending.push(hImpSubdividedCell(x, y, z));
						pendingVector.push_back(hImpSubdividedCell(x, y, z));
						std::push_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);

						error += hImpSubdividedCell(x, y, z).error;
					}
				}
			}
		}

		// recalculate every 1000 step for precision
		step = (step + 1) % 1000;
		if (step == 0)
		{
			error = 0.0f;
			for (auto& cell : pendingVector)
			{
				error += cell.error;
			}
		}
	}
}
#endif