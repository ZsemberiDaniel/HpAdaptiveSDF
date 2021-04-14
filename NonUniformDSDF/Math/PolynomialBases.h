#ifndef POLYNOMIALBASES_H
#define POLYNOMIALBASES_H
#include <functional>
#include <string>
#include "GaussPolynomialGenerator.h"
#include "LSQPolyGenerator.h"

#include "AverageIntegralEvaluator.h"
#include "QuadratureEvaluator.h"

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
	Polynomial::Type resultingPolynomialType;
};

inline PolynomialBase approxTypes[] = {
	// GAUSS QUADRATURE WITH LEGENDRE
	PolynomialBase {
		0,
		"Gauss Quadrature - normalized Legendre",
		"Gauss",
		"evalPolynom_normLagrange",
		 [](std::unique_ptr<Octree<Cell>>& octree, OctreeGenerator::ConstructionParameters& constr, SDFBase* sdfFunc)
		 {
			static GaussPolynomialGenerator generator(std::make_shared<QuadratureEvaluator>());

			OctreeGenerator::constructField<GaussPolynomialGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 },
		Polynomial::Type::LEGENDRE_NORMALIZED
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
		 },
		Polynomial::Type::LEGENDRE
	}, 

	// GAUSS QUADRATURE WITH LEGENDRE
	PolynomialBase {
		2,
		"Averaging Quadrature - normalized Legendre",
		"Averaging",
		"evalPolynom_normLagrange",
		 [](std::unique_ptr<Octree<Cell>>& octree, OctreeGenerator::ConstructionParameters& constr, SDFBase* sdfFunc)
		 {
			static GaussPolynomialGenerator generator(std::make_shared<AverageIntegralEvaluator>());

			OctreeGenerator::constructField<GaussPolynomialGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 },
		Polynomial::Type::LEGENDRE_NORMALIZED
	},

	// GAUSS QUADRATURE WITH LEGENDRE GPU edition
	PolynomialBase {
		3,
		"[GPU] Gauss Quadrature - normalized Legendre",
		"Gauss",
		"evalPolynom_normLagrange",
		[](std::unique_ptr<Octree<Cell>>& octree, OctreeGenerator::ConstructionParameters& constr, SDFBase* sdfFunc)
		{
			OctreeGenerator::constructFieldGPU(
				octree,
				constr,
				*sdfFunc);
		},
		Polynomial::Type::LEGENDRE_NORMALIZED
	}
};
#endif