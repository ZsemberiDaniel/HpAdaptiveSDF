#ifndef POLYNOMIALGENERATOR_H
#define POLYNOMIALGENERATOR_H
#include <vector>
#include "../Structures.h"
#include "../Math/Polynomial.h"

class PolynomialGenerator
{
public:
	/* Returns 9 polynomials for each defined bbox. 0 for the p improvement
	* and 1-8 indices for the h improvement.
	*/
	template<typename sdf>
	std::vector<Polynomial> fitPolynomials(std::vector<std::pair<BoundingBox, int>>& bboxes, sdf& sdfFunction)
	{
		std::vector<Polynomial> p;
		return p;
	}
};
#endif