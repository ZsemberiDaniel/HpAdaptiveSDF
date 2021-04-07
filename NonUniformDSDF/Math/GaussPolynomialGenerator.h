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
	Polynomial fitPolynomial(const BoundingBox& bbox, int degree, sdf& sdfFunction);

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
};

/* Equation 5 */
template<typename sdf>
Polynomial GaussPolynomialGenerator::fitPolynomial(const BoundingBox& bbox, int degree, sdf& sdfFunction)
{
	Polynomial polynomial(degree);

	int m = 0;
	for (int i = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				polynomial(m) = quadratureEvaluator->evaluateIntegral(20,
					[&bbox, i, k, j, this, &sdfFunction](glm::vec3 p)
					{
						return sdfFunction(p) * shiftedNormalizedLegendre(bbox, glm::ivec3(i, k, j), p);
					}, bbox.min, bbox.max);
			}
		}
	}

	return polynomial;
}
#endif
