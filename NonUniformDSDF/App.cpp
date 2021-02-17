#include "App.h"

#include <glm/gtx/transform.hpp>

using namespace df;

const float gaussPoints1D[20][20] = {
	{0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{-0.57735027,0.57735027,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,-0.77459667,0.77459667,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{-0.33998104,-0.86113631,0.33998104,0.86113631,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,-0.53846931,-0.90617985,0.53846931,0.90617985,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.23861919,0.66120939,0.93246951,-0.23861919,-0.66120939,-0.93246951,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,0.40584515,0.74153119,0.94910791,-0.40584515,-0.74153119,-0.94910791,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{-0.18343464,-0.52553241,-0.79666648,-0.96028986,0.18343464,0.52553241,0.79666648,0.96028986,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,-0.32425342,-0.61337143,-0.83603111,-0.96816024,0.32425342,0.61337143,0.83603111,0.96816024,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{-0.14887434,-0.43339539,-0.67940957,-0.86506337,-0.97390653,0.14887434,0.43339539,0.67940957,0.86506337,0.97390653,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,-0.26954316,-0.51909613,-0.73015201,-0.88706260,-0.97822866,0.26954316,0.51909613,0.73015201,0.88706260,0.97822866,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.12523341,0.36783150,0.58731795,0.76990267,0.90411726,0.98156063,-0.12523341,-0.36783150,-0.58731795,-0.76990267,-0.90411726,-0.98156063,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,0.23045832,0.44849275,0.64234934,0.80157809,0.91759840,0.98418305,-0.23045832,-0.44849275,-0.64234934,-0.80157809,-0.91759840,-0.98418305,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.10805495,0.31911237,0.51524864,0.68729290,0.82720132,0.92843488,0.98628381,-0.10805495,-0.31911237,-0.51524864,-0.68729290,-0.82720132,-0.92843488,-0.98628381,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,0.20119409,0.39415135,0.57097217,0.72441773,0.84820658,0.93727339,0.98799252,-0.20119409,-0.39415135,-0.57097217,-0.72441773,-0.84820658,-0.93727339,-0.98799252,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.09501251,0.28160355,0.45801678,0.61787624,0.75540441,0.86563120,0.94457502,0.98940093,-0.09501251,-0.28160355,-0.45801678,-0.61787624,-0.75540441,-0.86563120,-0.94457502,-0.98940093,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.00000000,0.17848418,0.35123176,0.51269054,0.65767116,0.78151400,0.88023915,0.95067552,0.99057548,-0.17848418,-0.35123176,-0.51269054,-0.65767116,-0.78151400,-0.88023915,-0.95067552,-0.99057548,0.00000000,0.00000000,0.00000000},
	{-0.08477501,-0.25188623,-0.41175116,-0.55977083,-0.69168704,-0.80370496,-0.89260247,-0.95582395,-0.99156517,0.08477501,0.25188623,0.41175116,0.55977083,0.69168704,0.80370496,0.89260247,0.95582395,0.99156517,0.00000000,0.00000000},
	{0.00000000,-0.16035865,-0.31656410,-0.46457074,-0.60054530,-0.72096618,-0.82271466,-0.90315590,-0.96020815,-0.99240684,0.16035865,0.31656410,0.46457074,0.60054530,0.72096618,0.82271466,0.90315590,0.96020815,0.99240684,0.00000000},
	{0.07652652,0.22778585,0.37370609,0.51086700,0.63605368,0.74633191,0.83911697,0.91223443,0.96397193,0.99312860,-0.07652652,-0.22778585,-0.37370609,-0.51086700,-0.63605368,-0.74633191,-0.83911697,-0.91223443,-0.96397193,-0.99312860}
};

const float gaussCoeffs1D[20][20] = {
	{2.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{1.00000000,1.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.88888889,0.55555556,0.55555556,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.65214515,0.34785485,0.65214515,0.34785485,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.56888889,0.47862867,0.23692689,0.47862867,0.23692689,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.46791393,0.36076157,0.17132449,0.46791393,0.36076157,0.17132449,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.41795918,0.38183005,0.27970539,0.12948497,0.38183005,0.27970539,0.12948497,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.36268378,0.31370665,0.22238103,0.10122854,0.36268378,0.31370665,0.22238103,0.10122854,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.33023936,0.31234708,0.26061070,0.18064816,0.08127439,0.31234708,0.26061070,0.18064816,0.08127439,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.29552422,0.26926672,0.21908636,0.14945135,0.06667134,0.29552422,0.26926672,0.21908636,0.14945135,0.06667134,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.27292509,0.26280454,0.23319376,0.18629021,0.12558037,0.05566857,0.26280454,0.23319376,0.18629021,0.12558037,0.05566857,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.24914705,0.23349254,0.20316743,0.16007833,0.10693933,0.04717534,0.24914705,0.23349254,0.20316743,0.16007833,0.10693933,0.04717534,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.23255155,0.22628318,0.20781605,0.17814598,0.13887351,0.09212150,0.04048400,0.22628318,0.20781605,0.17814598,0.13887351,0.09212150,0.04048400,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.21526385,0.20519846,0.18553840,0.15720317,0.12151857,0.08015809,0.03511946,0.21526385,0.20519846,0.18553840,0.15720317,0.12151857,0.08015809,0.03511946,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.20257824,0.19843149,0.18616100,0.16626921,0.13957068,0.10715922,0.07036605,0.03075324,0.19843149,0.18616100,0.16626921,0.13957068,0.10715922,0.07036605,0.03075324,0.00000000,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.18945061,0.18260342,0.16915652,0.14959599,0.12462897,0.09515851,0.06225352,0.02715246,0.18945061,0.18260342,0.16915652,0.14959599,0.12462897,0.09515851,0.06225352,0.02715246,0.00000000,0.00000000,0.00000000,0.00000000},
	{0.17944647,0.17656271,0.16800410,0.15404576,0.13513637,0.11188385,0.08503615,0.05545953,0.02414830,0.17656271,0.16800410,0.15404576,0.13513637,0.11188385,0.08503615,0.05545953,0.02414830,0.00000000,0.00000000,0.00000000},
	{0.16914238,0.16427648,0.15468468,0.14064291,0.12255521,0.10094204,0.07642573,0.04971455,0.02161601,0.16914238,0.16427648,0.15468468,0.14064291,0.12255521,0.10094204,0.07642573,0.04971455,0.02161601,0.00000000,0.00000000},
	{0.16105445,0.15896884,0.15276604,0.14260670,0.12875396,0.11156665,0.09149002,0.06904454,0.04481423,0.01946179,0.15896884,0.15276604,0.14260670,0.12875396,0.11156665,0.09149002,0.06904454,0.04481423,0.01946179,0.00000000},
	{0.15275339,0.14917299,0.14209611,0.13168864,0.11819453,0.10193012,0.08327674,0.06267205,0.04060143,0.01761401,0.15275339,0.14917299,0.14209611,0.13168864,0.11819453,0.10193012,0.08327674,0.06267205,0.04060143,0.01761401}
};

float SDF(glm::vec3 p)
{
	// TODO: plane, stop at 1st

	// sphere in -2, -2, -2 with radius 1
	return glm::length(p - glm::vec3(-2, -2, -2)) - 1.0f;
}

/* Returns points and coefficients for Gaussian-Legendre quadratics
*/
GaussCoeffPoints gaussPoints(int pointCount)
{
	GaussCoeffPoints output;
	if (pointCount > 20 || pointCount <= 0)
	{
		std::cerr << "Point count for gaussian quadratic is maximum 20 and minimum 1!" << std::endl;
		return output;
	}

	output.points.resize(pointCount * pointCount * pointCount);
	output.coeffs.resize(pointCount * pointCount * pointCount);

	for (int i = 0, m = 0; i < pointCount; i++)
	{
		for (int k = 0; k < pointCount; k++)
		{
			for (int j = 0; j < pointCount; j++, m++)
			{
				output.coeffs[m] = gaussCoeffs1D[pointCount - 1][i] * 
								   gaussCoeffs1D[pointCount - 1][j] *
								   gaussCoeffs1D[pointCount - 1][k];
				output.points[m] = glm::vec3(gaussPoints1D[pointCount - 1][i],
											 gaussPoints1D[pointCount - 1][k],
											 gaussPoints1D[pointCount - 1][j]);
			}
		}
	}

	return output;
}

/* Evaluates the Gaussian-Legendre quadratics for the given R^3 -> R function
*/
template<typename F>
float evaluateGaussQuadratic(GaussCoeffPoints gcp, F function)
{
	float value = 0.0f;
	for (int i = 0; i < gcp.coeffs.size(); i++)
	{
		value += gcp.coeffs[i] * function(gcp.points[i]);
	}

	return value;
}

void testGaussPoints() 
{
	GaussCoeffPoints gcp = gaussPoints(15);

	auto eval = [&gcp](std::function<float(glm::vec3)> fun, float realVal) 
	{
		float value = 0.0f;
		for (int i = 0; i < gcp.coeffs.size(); i++)
		{
			value += gcp.coeffs[i] * fun(gcp.points[i]);
		}
		std::cout << "Calculated by gaussian: " << value << "\tReal: " << realVal << std::endl;
	};
	
	// x^2 + y^2 + z^2
	eval([](glm::vec3 x) { return glm::dot(x, x); }, 8);

	// sin(x^2) + y + z^3
	eval([](glm::vec3 p) { return sinf(p.x * p.x) + p.y + p.z * p.z * p.z; }, 2.48215);
}

/* Equation 4
*/
float shiftedNormalizedLegendre(const BoundingBox& cell, glm::ivec3 degrees, glm::vec3 p) 
{
	/*glm::vec3 shiftedNormalizedP = 2.0f / glm::vec3(cell.max - cell.min) * p 
							       - glm::vec3((cell.max + cell.min) / (cell.max - cell.min));*/
	glm::vec3 shiftedNormalizedCoeff = (2.0f * glm::vec3(degrees) + 1.0f) / glm::vec3(cell.max - cell.min);

	float value = sqrtf(shiftedNormalizedCoeff.x) * std::legendref(degrees.x, p.x);
	value *= sqrtf(shiftedNormalizedCoeff.y) * std::legendref(degrees.y, p.y);
	value *= sqrtf(shiftedNormalizedCoeff.z) * std::legendref(degrees.z, p.z);

	return value;
}

int getCoeffCount(int degree) 
{
	return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
}

/* Equation 5
*/
Polynomial fitPolynomial(const BoundingBox& cell, int degree)
{
	Polynomial polynomial;
	// taken this from the paper: n_c, section 3.3
	polynomial.coeffs.resize(getCoeffCount(degree));

	GaussCoeffPoints gcp = gaussPoints(4 * degree);

	int m = 0;
	for (int i = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				polynomial.coeffs[m] = evaluateGaussQuadratic(
					gcp,
					[cell, i, k, j](glm::vec3 p) -> float {
						return SDF(cell.min + (p * 0.5f + 0.5f)) * 
							   shiftedNormalizedLegendre(cell, glm::ivec3(i, k, j), p);
					});
			}
		}
	}

	return polynomial;
}

double estimateError(Polynomial polynomial)
{
	double error = 0.0;

	for (auto& coeff : polynomial.coeffs)
	{
		error += coeff * coeff;
	}

	return error;
}

void App::constructField(Grid& grid, int maxDegree, int maxLevel, float errorThreshold)
{
	vector3d<Cell> initialCells(grid.initialCellCount);
	vector3d<Octree<Cell>::Leaf*> generatedCells(grid.initialCellCount);

	float error = 0.0f;
	// TODO: make pending a non-pair, only with the Cell and custom comparator
	std::priority_queue< std::pair<float, Cell> > pending;

	for (int x = 0; x < grid.initialCellCount; x++)
	{
		for (int y = 0; y < grid.initialCellCount; y++)
		{
			for (int z = 0; z < grid.initialCellCount; z++)
			{
				glm::vec3 cellCoord = grid.minPos + grid.cellSize * glm::vec3(x, y, z);
				BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + grid.cellSize };

				Polynomial poly = fitPolynomial(bbox, 2);
				float currentError = estimateError(poly);

				Cell cell = Cell{bbox, poly, currentError, 2, 1};

				error += currentError;

				initialCells(x, y, z) = cell;
			}
		}
	}
	octree = Octree<Cell>(grid.initialCellCount, initialCells, generatedCells);
	for (int x = 0; x < grid.initialCellCount; x++)
	{
		for (int y = 0; y < grid.initialCellCount; y++)
		{
			for (int z = 0; z < grid.initialCellCount; z++)
			{
				initialCells(x, y, z).octreeLeaf = generatedCells(x, y, z);
				pending.push(std::make_pair(initialCells(x, y, z).error, initialCells(x, y, z)));
			}
		}
	}

	while (!pending.empty() && error > errorThreshold)
	{
		auto popped = pending.top();
		pending.pop();

		Cell& currentCell = popped.second;
		if (currentCell.degree >= maxDegree && currentCell.level >= maxLevel) continue;

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
		if (currentCell.degree < maxDegree)
		{
			pImprovementPoly = fitPolynomial(currentCell.bbox, currentCell.degree + 1);
			pImprovementError = estimateError(pImprovementPoly);

			// Equation (8), section 3.6
			pImprovement = 1.0f / (getCoeffCount(currentCell.degree + 1) - getCoeffCount(currentCell.degree)) *
				(currentCell.error - 8 * pImprovementError);
		}

		// h-improvement
		// subdivide cell into 2x2x2
		if (currentCell.level < maxLevel)
		{
			// max error for subdivided cells, needed for eq. (9)
			float maxError = std::numeric_limits<float>::min();
			// how big a subdivided cube's side is
			float subdividedCubeSize = (currentCell.bbox.max.x - currentCell.bbox.min.x) / 2.0f;

			for (int i = 0; i < 2; i++)
			{
				for (int k = 0; k < 2; k++)
				{
					for (int j = 0; j < 2; j++)
					{
						// the subdivided cubes' coord is shifted from the bigger cube's coord
						glm::vec3 cellCoord = currentCell.bbox.min + glm::vec3(i, k, j) * glm::vec3(subdividedCubeSize);
						BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + glm::vec3(subdividedCubeSize) };

						// polynom for new cell + making new cell
						Polynomial poly = fitPolynomial(bbox, currentCell.degree);
						float currentError = estimateError(poly);

						Cell cell = Cell{ bbox, poly, currentError, currentCell.degree, currentCell.level + 1 };

						hImpSubdividedCell(i, j, k) = cell;
					}
				}
			}

			// equation (9), section 3.6
			hImprovement = 1.0f / 7.0f / getCoeffCount(currentCell.degree) * (currentCell.error - 8 * maxError);
		}

		// deciding in favor of p-improvement
		if (pImprovement > hImprovement && pImprovement > 0)
		{
			currentCell.degree++;
			currentCell.poly  = pImprovementPoly;
			currentCell.error = pImprovementError;

			pending.push(std::make_pair(currentCell.error, currentCell));
		}
		// doing h-improvement otherwise
		else if (pImprovement < hImprovement && hImprovement > 0)
		{
			vector3d<Octree<Cell>::Leaf*> leaves(2);
			currentCell.octreeLeaf->subdivide(hImpSubdividedCell, leaves);

			for (int i = 0; i < 2; i++)
			{
				for (int k = 0; k < 2; k++)
				{
					for (int j = 0; j < 2; j++)
					{
						hImpSubdividedCell(i, k, j).octreeLeaf = leaves(i, k, j);
						pending.push(std::make_pair(hImpSubdividedCell(i, k, j).error, hImpSubdividedCell(i, k, j)));
					}
				}
			}
		}
	}
}


App::App(df::Sample& s) : sam(s), noVao(GL_TRIANGLES, 3)
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitShaders();

	sam.AddHandlerClass(state.cam);

	// testGaussPoints();
	Grid grid = Grid{ glm::vec3(0), 2, glm::vec3(1) };
	constructField(grid);

	GL_CHECK;
}

App::~App()
{
}

void App::Update()
{
	state.cam.Update();
}

void App::DrawOctree(df::ShaderProgramEditorVF& program, const Octree<Cell>::Node* currentNode)
{
	if (currentNode->type() == Octree<Cell>::BranchNode)
	{
		auto* branch = reinterpret_cast<const Octree<Cell>::Branch*>(currentNode);
		for (int i = 0; i < 8; i++)
		{
			DrawOctree(program, branch->child(i));
		}
	}
	else
	{
		assert(currentNode->type() == Octree<Cell>::LeafNode);
		auto* branch = reinterpret_cast<const Octree<Cell>::Leaf*>(currentNode);

		df::Backbuffer << program << "MVP" << state.cam.GetViewProj() * glm::translate(glm::vec3(branch->value().bbox.min));
	}
}

void App::Render()
{
	Backbuffer << Clear(1, 1, 1, 1);
	auto& prog = sdfProgram;

	DrawOctree(cubeWireProgram, octree.root());

	GL_CHECK;
}

void App::RenderGUI()
{
	if (!state.enableGUI) return;
	sdfProgram.Render();
	state.cam.RenderUI();
}

void App::InitShaders()
{
	std::cout << "Compiling cube wire program...  ";
	cubeWireProgram
		<< "Shaders/cube.vert"_vs
		<< "Shaders/cube.frag"_fs
		<< LinkProgram;
	std::cout << cubeWireProgram.GetErrors();

	std::cout << "Compiling cube mesh program...  ";
	meshProgram
		<< "Shaders/mesh.vert"_vs
		<< "Shaders/mesh.frag"_fs
		<< LinkProgram;
	std::cout << meshProgram.GetErrors();

	std::cout << "Compiling mesh program...  ";
	flatMeshProgram
		<< "Shaders/mesh.vert"_vs
		<< "Shaders/flatMesh.geom"_geom
		<< "Shaders/mesh.frag"_fs
		<< LinkProgram;
	std::cout << flatMeshProgram.GetErrors();

	std::cout << "Compiling sdf trace program...  ";
	sdfProgram
		<< "Shaders/Math/box_plane_intersection.glsl"_vs
		<< "Shaders/cube_solid.vert"_vs

		<< "Shaders/render_main.frag"_fs
		<< "Shaders/uniforms.glsl"_fs
		<< "Shaders/common.glsl"_fs
		<< "Shaders/Math/box_ray_intersection.glsl"_fs
		<< "Shaders/Shade/ambient_occlusion.glsl"_fs
		<< "Shaders/Shade/brdf.glsl"_fs
		<< "Shaders/Shade/basic_shade.glsl"_fs
		<< "Shaders/main_cube.glsl"_fs
		<< "Shaders/sdf.glsl"_fs
		<< "Shaders/Math/primitives.glsl"_fs
		<< "Shaders/SDF/weightBlend.glsl"_fs
		<< "Shaders/SDF/sample3d.glsl"_fs

		<< "Shaders/SDFScenes/torus_pipes.glsl"_fs
		<< "Shaders/SDF/sdfProceduralScene.glsl"_fs
//		<< "Shaders/SDF/sdf0thOrder.glsl"_fs
//		<< "Shaders/SDF/sdf1stOrder.glsl"_fs
		
		<< LinkProgram;
	std::cout << sdfProgram.GetErrors();

	
}
