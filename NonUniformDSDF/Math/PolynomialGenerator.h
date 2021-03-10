#pragma once
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
