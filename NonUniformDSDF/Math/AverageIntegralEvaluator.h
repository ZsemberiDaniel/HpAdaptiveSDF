#ifndef AVERAGEINTEGRALEVALUATOR_H
#define AVERAGEINTEGRALEVALUATOR_H

#include "IntegralEvaluator.h"
#include <glm/glm.hpp>

class AverageIntegralEvaluator : public IntegralEvaluator
{

public:
	AverageIntegralEvaluator() : IntegralEvaluator() 
	{
	}
	/*
	 * Evaluates the given function over the given integral (defaults to [-1;1]^3)
	 */
	virtual float evaluateIntegral(int pointCount, std::function<float(glm::vec3)> function,
		glm::vec3 intervalStarting = glm::vec3(-1), glm::vec3 intervalEnding = glm::vec3(1)) override
	{
		//Create MATLAB data array factory
		// matlab::data::ArrayFactory factory;
		
		

		// simpson's rule only works for even numbers
		if (pointCount % 2 == 1) pointCount++;

		int subIntervalCount = pointCount - 1;
		glm::vec3 intervalSize = intervalEnding - intervalStarting;
		glm::vec3 subIntervalSize = intervalSize / (float)subIntervalCount;
		float value = 0.0f;

		auto getSimpsonCoeff = [&subIntervalSize, subIntervalCount](glm::ivec3 coeffIds) {
			glm::vec3 result = glm::vec3(0);

			if (coeffIds.x == 0 || coeffIds.x == subIntervalCount) result.x = subIntervalSize.x / 3.0f;
			else if (coeffIds.x % 2 == 0) result.x = 2.0f * subIntervalSize.x / 3.0f;
			else if (coeffIds.x % 2 == 1) result.x = 4.0f * subIntervalSize.x / 3.0f;

			if (coeffIds.y == 0 || coeffIds.y == subIntervalCount) result.y = subIntervalSize.y / 3.0f;
			else if (coeffIds.y % 2 == 0) result.y = 2.0f * subIntervalSize.y / 3.0f;
			else if (coeffIds.y % 2 == 1) result.y = 4.0f * subIntervalSize.y / 3.0f;

			if (coeffIds.z == 0 || coeffIds.z == subIntervalCount) result.z = subIntervalSize.z / 3.0f;
			else if (coeffIds.z % 2 == 0) result.z = 2.0f * subIntervalSize.z / 3.0f;
			else if (coeffIds.z % 2 == 1) result.z = 4.0f * subIntervalSize.z / 3.0f;

			return result;
		};

		// composite simpson's rule
		for (int x = 0; x < pointCount; x++)
		{
			for (int y = 0; y < pointCount; y++)
			{
				for (int z = 0; z < pointCount; z++)
				{
					// shift the quadratic point into the input integral
					glm::vec3 shiftedPoint = glm::vec3(x, y, z) / (float)subIntervalCount * intervalSize + intervalStarting;
					float functionVal = function(shiftedPoint);
					glm::vec3 coeffs = getSimpsonCoeff(glm::ivec3(x, y, z));

					value += coeffs.x * coeffs.y * coeffs.z * functionVal;
				}
			}
		}

		return value;
		return value * intervalSize.x * intervalSize.y * intervalSize.z / 8.0f;
	}
};

#endif
