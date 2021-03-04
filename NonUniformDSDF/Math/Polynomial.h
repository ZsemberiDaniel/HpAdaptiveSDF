#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <cmath>
#include "../Eigen/Dense"

/*
 * Represents a polynomial which has one coefficient up to a maximum degree.
 */
class Polynomial
{
public:
	float operator()(glm::vec3 p);
	
	// TODO: remove () operator
	float& operator()(int i);
	const float& operator()(int i) const;

	float& operator[](int i);
	const float& operator[](int i) const;
	Polynomial& operator= (const Polynomial& poly);
	
	Polynomial(int degree = 0);
	Polynomial(const Polynomial& poly);

	int getDegree() const { return degree; }
	int coeffCount() const;
	float coefficientBasedError() const;
	// Prints the the coefficients of the polynomial that are > epsilon
	void print(float epsilon = 0.0000001f) const;

	static int calculateCoeffCount(int degree)
	{
		return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
	}
	
private:
	// To see how they are ordered in this array check out the evaluation operator()
	std::vector<float> coeffs;
	int degree;

	int calculateCoeffCount()
	{
		return calculateCoeffCount(degree);
	}
};
