#ifndef POLYNOMIALBASES_H
#define POLYNOMIALBASES_H
#include <functional>
#include <string>
#include "GaussPolynomialGenerator.h"
#include "LSQPolyGenerator.h"

#include "../Octree/octree.h"
#include "../Octree/octreeGenerator.h"
#include "../SDF/SDFBase.h"

struct PolynomialBase
{
	int id;
	std::string name;
	std::string shortUniqueName;
	std::string shaderEvalFunctionName;
	std::function<void(std::unique_ptr<Octree<Cell>>&, OctreeGenerator::ConstructionParameters&, SDFBase*)> cpuConstruction;
};

inline PolynomialBase approxTypes[2] = {
	// GAUSS QUADRATURE WITH LEGENDRE
	PolynomialBase {
		0,
		"Gauss Quadrature - normalized Legendre",
		"Gauss",
		"evalPolynom_normLagrange",
		 [](std::unique_ptr<Octree<Cell>>& octree, OctreeGenerator::ConstructionParameters& constr, SDFBase* sdfFunc)
		 {
			static GaussPolynomialGenerator generator;

			OctreeGenerator::constructField<GaussPolynomialGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 }
	},

	// LEAST SQUARES WITH LEGENDRE
	PolynomialBase {
		1,
		"LSQ - Legendre",
		"LSQ",
		"evalPolynom_lagrange",
		 [](std::unique_ptr<Octree<Cell>>& octree, OctreeGenerator::ConstructionParameters& constr, SDFBase* sdfFunc)
		 {
			static LSQPolyGenerator generator;

			OctreeGenerator::constructField<LSQPolyGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 }
	}
};
#endif