#ifndef INTEGRALEVALUATOR_H
#define INTEGRALEVALUATOR_H

#include <functional>
#include <glm/glm.hpp>

class IntegralEvaluator
{
public:
	IntegralEvaluator() { }
	/*
	 * Evaluates the given function over the given integral (defaults to [-1;1]^3)
	 */
	virtual float evaluateIntegral(int pointCount, std::function<float(glm::vec3)> function, 
		glm::vec3 intervalStarting = glm::vec3(-1), glm::vec3 intervalEnding = glm::vec3(1)) = 0;
};

#endif
