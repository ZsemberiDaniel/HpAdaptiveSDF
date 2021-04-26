#ifndef LSQPOLYGENERATOR_H
#define LSQPOLYGENERATOR_H

#include <iostream>
#include "PolynomialGenerator.h"
#include "../Structures.h"
#include "../Eigen/Dense"

class LSQPolyGenerator : public PolynomialGenerator
{
public:
	LSQPolyGenerator(int pointCount = 3, int maxDegree = 5)
		: maxDegree(maxDegree)
	{
		if (pointCount < 2)
		{
			std::cout << "LSQPolyGenerator point count was set to 2 instead of " << pointCount << std::endl;
			pointCount = 2;
		}

		points.reserve(pointCount * pointCount * pointCount);
		pseudoInverseMatrices.reserve(maxDegree + 1);
		
		// populate linearly interpolated polynomials
		for (int i = 0; i < pointCount; i++)
		{
			for (int j = 0; j < pointCount; j++)
			{
				for (int k = 0; k < pointCount; k++)
				{
					points.emplace_back(glm::vec3(i, j, k) / (pointCount - 1.0f) * 2.0f - 1.0f);
				}
			}
		}

		calculatePseudoInverseMatrices();
	}

	template<typename sdf>
	std::vector<Polynomial> fitPolynomials(std::vector<std::pair<BoundingBox, int>>& bboxes, sdf& sdfFunction);

private:
	int maxDegree;
	std::vector<glm::vec3> points;
	std::vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>> pseudoInverseMatrices;

	void calculatePseudoInverseMatrices()
	{
		for (int degree = 0; degree <= maxDegree; degree++)
		{
			pseudoInverseMatrices.emplace_back(Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>(Polynomial::calculateCoeffCount(degree), points.size()));
			auto& mat = pseudoInverseMatrices.back();

			std::vector<glm::vec3> base(degree + 1);
			int sid = 0;
			for(auto& samp : points)
			{
				// Legendre base functions
				// a = 1;
				// b = p;
				// float c = (1.0f / at) * ((2.0f * at - 1.0f) * p * b - (at - 1.0f) * a);
				
				base[0] = glm::vec3(1.0f);
				if (degree > 0) base[1] = samp;
				for(int i = 2; i <= degree; ++i)
				{
					base[i] = (1.0f / i) * ((2.0f * i - 1.0f) * samp * base[i - 1] - (i - 1.0f) * base[i - 2]);
				}

				// monomial base functions
				/*base[0] = glm::vec3(1.0f);
				for (int i = 1; i <= degree; ++i)
				{
					base[i] = base[i - 1] * samp; 
				}*/

				for (int i = 0, m = 0; i <= degree; i++)
				{
					for (int k = 0; i + k <= degree; k++)
					{
						for (int j = 0; i + k + j <= degree; j++, m++)
						{
							mat(m, sid) = base[i].x * base[k].y * base[j].z;
						}
					}
				}
				sid++;
			}

			mat = ((mat * mat.transpose()).inverse() * mat).eval();
		}
	}

	template<typename sdf>
	Polynomial fitOnePoly(BoundingBox& bbox, int degree, sdf& sdfFunction);
};

template<typename sdf>
Polynomial LSQPolyGenerator::fitOnePoly(BoundingBox& bbox, int degree, sdf& sdfFunction)
{
	Polynomial polynomial(degree);

	auto& mat = pseudoInverseMatrices[degree];

	int sampleId = 0;
	for (auto& samp : points)
	{
		float sampleVal = sdfFunction((samp * 0.5f + 0.5f) * bbox.size() + bbox.min);
		for (int i = 0; i < polynomial.coeffCount(); i++)
		{
			polynomial[i] += mat(i, sampleId) * sampleVal;
		}
		sampleId++;
	}

	return polynomial;
}


template<typename sdf>
std::vector<Polynomial> LSQPolyGenerator::fitPolynomials(std::vector<std::pair<BoundingBox, int>>& bboxes, sdf& sdfFunction)
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
