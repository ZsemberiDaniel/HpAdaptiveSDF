#ifndef POLYNOMIALGENERATOR_H
#define POLYNOMIALGENERATOR_H
#include "../Structures.h"
#include "../Math/Polynomial.h"

class PolynomialGenerator
{
public:
	template<typename sdf>
	Polynomial fitPolynomial(const BoundingBox& bbox, int degree, sdf& sdfFunction)
	{
		Polynomial p;
		return p;
	}
};
#endif