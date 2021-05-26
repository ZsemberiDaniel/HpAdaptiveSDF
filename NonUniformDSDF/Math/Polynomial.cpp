#include "Polynomial.h"

#include <iomanip>
#include <iostream>
#include <glm/common.hpp>
#include <glm/exponential.hpp>


Polynomial::Polynomial(int _degree) :
	degree(_degree)
{
	coeffs.resize(calculateCoeffCount(), 0.0f);
}

Polynomial::Polynomial(const Polynomial& poly) :
	Polynomial(poly.degree)
{
	coeffs = poly.coeffs;
}

Polynomial& Polynomial::operator=(const Polynomial& poly)
{
	degree = poly.degree;
	coeffs.resize(calculateCoeffCount(), 0.0f);
	coeffs = poly.coeffs;

	return *this;
}

int Polynomial::coeffCount() const
{
	return coeffs.size();
}

float Polynomial::coefficientBasedError() const
{
	float error = 0.0;

	// only the coefficients where i + j + k = p
	for (int i = 0, m = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				if (i + j + k == degree)
				{
					error += coeffs[m] * coeffs[m];
				}
			}
		}
	}

	return error;
}

void Polynomial::print(float epsilon) const
{
	int m = 0;
	std::cout << std::setprecision(4);
	for (int i = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				if (glm::abs(coeffs[m]) < epsilon) continue;

				std::cout << (i + k + j == 0 ? "" : " + ") << coeffs[m];
				
				if (i == 1) std::cout << " * Lx";
				else if (i != 0) std::cout << " * Lx^" << i;

				if (k == 1) std::cout << " * Ly";
				else if (k != 0) std::cout << " * Ly^" << k;

				if (j == 1) std::cout << " * Lz";
				else if (j != 0) std::cout << " * Lz^" << j;
			}
		}
	}

	std::cout << std::endl;
}

float& Polynomial::operator[](int i)
{
	return coeffs[i];
}

const float& Polynomial::operator[](int i) const
{
	return coeffs[i];
}

float Polynomial::operator()(glm::vec3 p, Type type, glm::vec3 bboxSize) const
{
	float ret = 0.0f;

	for (int i = 0, m = 0; i <= degree; i++)
	{
		for (int k = 0; i + k <= degree; k++)
		{
			for (int j = 0; i + k + j <= degree; j++, m++)
			{
				switch (type)
				{
				case LEGENDRE:
					ret += coeffs[m] * std::legendref(i, p.x) * std::legendref(k, p.y) * std::legendref(j, p.z);
					break;
				case LEGENDRE_NORMALIZED:
					glm::vec3 a = glm::sqrt((2.0f * glm::vec3(i, k, j) + 1.0f) / bboxSize);
					ret += coeffs[m] * std::legendref(i, p.x) * std::legendref(k, p.y) * std::legendref(j, p.z) * a.x * a.y * a.z;
					break;
				case MONOMIAL:
					ret += coeffs[m] * pow(p.x, i) * pow(p.y, k) * pow(p.z, j);
					break;
				}
			}
		}
	}

	return ret;
}
