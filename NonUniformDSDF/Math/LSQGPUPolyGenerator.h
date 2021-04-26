#ifndef LSQGPUPOLYNOMIALGENERATOR_H
#define LSQSGPUPOLYNOMIALGENERATOR_H

#include "GPUPolyGenerator.h"

template<typename sdf>
class LSQGPUPolyGenerator : public GPUPolyGenerator<sdf>
{
public:
	LSQGPUPolyGenerator(int maxDegree, int maxK, sdf& sdfFunction) :
		GPUPolyGenerator<sdf>("Shaders/PolyGen/LSQPolyGen.comp", maxDegree, maxK, sdfFunction)
	{ }
};
#endif
