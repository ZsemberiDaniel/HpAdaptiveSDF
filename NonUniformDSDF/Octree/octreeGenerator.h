#ifndef OCTREEGENERATOR_H
#define OCTREEGENERATOR_H
#include <iostream>
#include <Dragonfly/editor.h>		 //inlcludes most features

#include "../Structures.h"
#include "octree.h"

#include "../Math/Polynomial.h"
#include "../constants.h"

class OctreeGenerator
{
public:
	struct ConstructionParameters
	{
		// Min pos in the world of the octree. Currently not working
		glm::vec3 minPos = glm::vec3(0);
		// Size in the world of the octree. Currently not working
		float sizeInWorld = 1.0f;
		// Maximum degree reachable by algorithm
		int maxDegree = 3;
		// Maximum octree level reachable by algorithm
		int maxLevel = 2;
		// Maximum absolute error sum obtained by algorithm
		float errorThreshold = 0.005f;
		// Use subdivision in algorithm?
		bool useHAdapt = true;
		// Raise poly degree in algorithm?
		bool usePAdapt = true;
		// when calculating on GPU, how many to group together
		int cellGroupSize = 1;
		// starting degree of the polynomials in cells
		int startDegree = 2;
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
	int K = std::max(params.cellGroupSize, 1);
	float gridCellSize = params.sizeInWorld / INITIAL_CELL_COUNT;

	auto queueCellCompare = [](const Cell& c1, const Cell& c2) -> bool
	{
		return c1.error < c2.error;
	};

	float error = 0.0f;
	std::vector<Cell> pendingVector;
	{
		vector3d<Cell> initialCells(INITIAL_CELL_COUNT);
		vector3d<std::shared_ptr<Octree<Cell>::Leaf>> generatedCells(INITIAL_CELL_COUNT);

		for (int x = 0; x < INITIAL_CELL_COUNT; x++)
		{
			for (int y = 0; y < INITIAL_CELL_COUNT; y++)
			{
				for (int z = 0; z < INITIAL_CELL_COUNT; z++)
				{
					glm::vec3 cellCoord = params.minPos + gridCellSize * glm::vec3(x, y, z);
					BoundingBox bbox = { cellCoord, cellCoord + gridCellSize };
					auto vec = std::vector<std::pair<BoundingBox, int>>{ {bbox, params.startDegree - 1} };

					Polynomial poly = polynomialGenerator.fitPolynomials(vec, sdfFunction)[0];

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
				}
			}
		}
	}

	octree->setBbox(params.minPos, params.sizeInWorld);
	octree->maxDegreeInLeaves = params.startDegree;

	int step = 0;
	while (!pendingVector.empty() && error > params.errorThreshold)
	{
		std::vector<Cell> currentCellGroup;
		std::vector<std::pair<BoundingBox, int>> polyData;
		for (int i = 0; i < K && !pendingVector.empty(); i++)
		{
			std::pop_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);
			Cell popped = pendingVector.back();
			pendingVector.pop_back();

			if (popped.degree() >= params.maxDegree && popped.level() >= params.maxLevel)
			{
				i--;
				continue;
			}
			polyData.push_back({ popped.bbox, popped.degree() });
			currentCellGroup.push_back(popped);
		}
		std::vector<Polynomial> generatedPolys = polynomialGenerator.fitPolynomials(polyData, sdfFunction);

		for (int cellIdInGroup = 0; cellIdInGroup < currentCellGroup.size(); cellIdInGroup++)
		{
			Cell& currentCell = currentCellGroup[cellIdInGroup];
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
				pImprovementPoly = generatedPolys[9 * cellIdInGroup];
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

				for (int x = 0, m = 0; x < 2; x++)
				{
					for (int y = 0; y < 2; y++)
					{
						for (int z = 0; z < 2; z++, m++)
						{
							// the subdivided cubes' coord is shifted from the bigger cube's coord
							glm::vec3 cellCoord = currentCell.bbox.min + glm::vec3(x, y, z) * glm::vec3(subdividedCubeSize);
							BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + glm::vec3(subdividedCubeSize) };

							Polynomial poly = generatedPolys[9 * cellIdInGroup + m + 1];
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
				pendingVector.push_back(std::move(currentCell));
				std::push_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);

				if (octree->maxDegreeInLeaves > currentCell.poly.getDegree())
				{
					octree->maxDegreeInLeaves = currentCell.poly.getDegree();
				}
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

							error += hImpSubdividedCell(x, y, z).error;
							// pending.push(hImpSubdividedCell(x, y, z));
							pendingVector.push_back(std::move(hImpSubdividedCell(x, y, z)));
							std::push_heap(pendingVector.begin(), pendingVector.end(), queueCellCompare);
						}
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