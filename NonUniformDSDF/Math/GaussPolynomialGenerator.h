#ifndef GAUSSPOLYNOMIALGENERATOR_H
#define GAUSSPOLYNOMIALGENERATOR_H

#include "PolynomialGenerator.h"
#include "../Structures.h"
#include "IntegralEvaluator.h"
#include <glm/glm.hpp>

class GaussPolynomialGenerator : public PolynomialGenerator
{
public:
	GaussPolynomialGenerator(std::shared_ptr<IntegralEvaluator> eval)
	{
		quadratureEvaluator.swap(eval);
	}

	template<typename sdf>
	std::vector<Polynomial> fitPolynomials(std::vector<std::pair<BoundingBox, int>>& bboxes, sdf& sdfFunction);

private:
	std::shared_ptr<IntegralEvaluator> quadratureEvaluator;
	float shiftedNormalizedLegendre(const BoundingBox& bbox, glm::ivec3 degrees, glm::vec3 p) const
	{
		glm::vec3 shiftedNormalizedP = 2.0f / glm::vec3(bbox.max - bbox.min) * p
			- glm::vec3((bbox.max + bbox.min) / (bbox.max - bbox.min));
		glm::vec3 shiftedNormalizedCoeff = glm::sqrt((2.0f * glm::vec3(degrees) + 1.0f) / bbox.size());

		float value = (shiftedNormalizedCoeff.x) * std::legendref(degrees.x, shiftedNormalizedP.x);
		value *= (shiftedNormalizedCoeff.y) * std::legendref(degrees.y, shiftedNormalizedP.y);
		value *= (shiftedNormalizedCoeff.z) * std::legendref(degrees.z, shiftedNormalizedP.z);

		return value;
	}

	template<typename sdf>
	Polynomial fitOnePoly(BoundingBox& bbox, int degree, sdf& sdfFunction);
};

/* Equation 5 */
template<typename sdf>
Polynomial GaussPolynomialGenerator::fitOnePoly(BoundingBox& bbox, int degree, sdf& sdfFunction)
{
	Polynomial polynomial(degree);

	int m = 0;
	for (int i = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				polynomial[m] = quadratureEvaluator->evaluateIntegral(20,
					[&bbox, i, k, j, this, &sdfFunction](glm::vec3 p)
					{
						return sdfFunction(p) * shiftedNormalizedLegendre(bbox, glm::ivec3(i, k, j), p);
					}, bbox.min, bbox.max);
			}
		}
	}

	return polynomial;
}


template<typename sdf>
std::vector<Polynomial> GaussPolynomialGenerator::fitPolynomials(std::vector<std::pair<BoundingBox, int>>& bboxes, sdf& sdfFunction)
{
	std::vector<Polynomial> polys(9 * bboxes.size());
	int i = 0;
	for (auto& pair : bboxes)
	{
		polys[9 * i + 0] = fitOnePoly(pair.first, pair.second + 1, sdfFunction);

		float subdividedCubeSize = pair.first.size().x / 2.0f;
		for (int x = 0, m = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int z = 0; z < 2; z++, m++)
				{
					// the subdivided cubes' coord is shifted from the bigger cube's coord
					glm::vec3 cellCoord = pair.first.min + glm::vec3(x, y, z) * glm::vec3(subdividedCubeSize);
					BoundingBox bbox = BoundingBox{ cellCoord, cellCoord + glm::vec3(subdividedCubeSize) };

					polys[9 * i + m + 1] = fitOnePoly(bbox, pair.second, sdfFunction);
				}
			}
		}

		i++;
	}

	return polys;
}
#endif
