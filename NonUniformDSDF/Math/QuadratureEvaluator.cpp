#include "QuadratureEvaluator.h"

QuadratureEvaluator::QuadratureEvaluator(int _maxPointCount) :
	IntegralEvaluator(), maxPointCount(_maxPointCount)
{
	// init the quadrature points so they don't need to be recalculated each time
	quadraturePoints.resize(maxPointCount + 1);

	// how many points there are in the quadrature
	for (int pointCount = 0; pointCount <= maxPointCount; pointCount++)
	{
		quadraturePoints[pointCount].coeffPoints.resize(pointCount * pointCount * pointCount);

		// init the coefficients and the points themselves
		for (int i = 0, m = 0; i < pointCount; i++)
		{
			for (int k = 0; k < pointCount; k++)
			{
				for (int j = 0; j < pointCount; j++, m++)
				{
					quadraturePoints[pointCount].coeffPoints[m].coefficient = 
						gaussCoeffs1D[pointCount - 1][i] *
						gaussCoeffs1D[pointCount - 1][k] *
						gaussCoeffs1D[pointCount - 1][j];
					quadraturePoints[pointCount].coeffPoints[m].point = glm::vec3(
						gaussPoints1D[pointCount - 1][i],
						gaussPoints1D[pointCount - 1][k],
						gaussPoints1D[pointCount - 1][j]);
				}
			}
		}
	}
}
