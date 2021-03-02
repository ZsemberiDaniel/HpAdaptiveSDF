#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <cmath>

/*
 * Represents a polynomial which has one coefficient up to a maximum degree.
 */
class Polynomial
{
public:
	float operator()(glm::vec3 p);
	float& operator()(int i);
	const float& operator()(int i) const;
	Polynomial& operator= (const Polynomial& poly);
	
	Polynomial(int degree = 0);
	Polynomial(const Polynomial& poly);
	
	int coeffCount() const;
	float coefficientBasedError() const;
	// Prints the the coefficients of the polynomial that are > epsilon
	void print(float epsilon = 0.0000001f) const;
	
private:
	// To see how they are ordered in this array check out the evaluation operator()
	std::vector<float> coeffs;
	int degree;

	int calculateCoeffCount()
	{
		return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
	}
};

