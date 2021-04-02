//#ifndef CHEBYSHEV_INTEGRAL_EVAL_H
//#define CHEBYSHEV_INTEGRAL_EVAL_H
//#include <utility>
//#include <vector>
//#include <glm/vec3.hpp>
//#include <iostream>
//#include "IntegralEvaluator.h"
//
//class ChebyshevIntegralEvaluator : public IntegralEvaluator
//{
//private:
//	const double chebyPoints1D[9] = {
//		0,
//		-0.911589307728434,
//		0.911589307728435,
//		-0.601018655380239,
//		-0.528761783057880,
//		0.601018655380240,
//		0.528761783057878,
//		-0.167906184214804,
//		0.167906184214804
//	};
//	const double chebyCoeffs1D[9] = {
//		
//	};
//
//	struct CoeffPoint
//	{
//		float coefficient;
//		glm::vec3 point;
//	};
//
//	struct QuadraturePoints
//	{
//		std::vector<CoeffPoint> coeffPoints;
//	};
//
//public:
//	QuadratureEvaluator(int _maxPointCount = 20);
//
//	/*
//	 * Evaluates the given function over the given integral (defaults to [-1;1]^3)
//	 */
//	virtual float evaluateIntegral(int pointCount, std::function<float(glm::vec3)> function,
//		glm::vec3 intervalStarting = glm::vec3(-1), glm::vec3 intervalEnding = glm::vec3(1)) override
//	{
//		if (pointCount > maxPointCount || pointCount <= 0)
//		{
//#if DEBUG
//			std::cerr << "Point count for gaussian quadratic is maximum " << maxPointCount << " and minimum 1! Point count may be capped!" << std::endl;
//#endif
//			if (pointCount <= 0) return 0.0f;
//		}
//
//		pointCount = std::min(pointCount, maxPointCount);
//
//		glm::vec3 intervalSize = intervalEnding - intervalStarting;
//		float value = 0.0f;
//		for (int i = 0; i < quadraturePoints[pointCount].coeffPoints.size(); i++)
//		{
//			// shift the quadratic point into the input integral
//			glm::vec3 shiftedPoint = (quadraturePoints[pointCount].coeffPoints[i].point * 0.5f + 0.5f) * intervalSize + intervalStarting;
//
//			value += quadraturePoints[pointCount].coeffPoints[i].coefficient * function(shiftedPoint);
//		}
//
//		return value * intervalSize.x * intervalSize.y * intervalSize.z / 8.0f;
//	}
//private:
//	int maxPointCount;
//	// ith contains coefficients and points for the quadrature with i*i*i points
//	std::vector<QuadraturePoints> quadraturePoints;
//};
//
//#endif