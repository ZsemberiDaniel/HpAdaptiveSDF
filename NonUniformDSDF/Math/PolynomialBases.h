#ifndef POLYNOMIALBASES_H
#define POLYNOMIALBASES_H
#include <functional>
#include <string>
#include "GaussPolynomialGenerator.h"
#include "GaussGPUPolyGenerator.h"
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
	bool calculatedOnGPU;
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

			constr.cellGroupSize = 1;
			OctreeGenerator::constructField<GaussPolynomialGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 },
		Polynomial::Type::LEGENDRE_NORMALIZED,
		false
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

			constr.cellGroupSize = 1;
			OctreeGenerator::constructField<LSQPolyGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 },
		Polynomial::Type::LEGENDRE,
		false
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

			constr.cellGroupSize = 1;
			OctreeGenerator::constructField<GaussPolynomialGenerator>(
				octree,
				generator,
				constr,
				*sdfFunc);
		 },
		Polynomial::Type::LEGENDRE_NORMALIZED,
		false
	},

	// GAUSS QUADRATURE WITH LEGENDRE GPU edition
	PolynomialBase {
		3,
		"[GPU] Gauss Quadrature - normalized Legendre",
		"GPUGauss",
		"evalPolynom_normLagrange",
		[](std::unique_ptr<Octree<Cell>>& octree, OctreeGenerator::ConstructionParameters& constr, SDFBase* sdfFunc)
		{
			GaussGPUPolyGenerator generator = GaussGPUPolyGenerator(constr.maxDegree, constr.cellGroupSize, *sdfFunc);

			OctreeGenerator::constructField(
				octree,
				generator,
				constr,
				*sdfFunc);
		},
		Polynomial::Type::LEGENDRE_NORMALIZED,
		true
	}
};
#endif