#ifndef GAUSSGPUPOLYNOMIALGENERATOR_H
#define GAUSSGPUPOLYNOMIALGENERATOR_H

#include "GPUPolyGenerator.h"

template<typename sdf>
class GaussGPUPolyGenerator : public GPUPolyGenerator<sdf>
{
public:
	GaussGPUPolyGenerator(int maxDegree, int maxK, sdf& sdfFunction) :
		GPUPolyGenerator<sdf>("Shaders/PolyGen/GaussPolyGen.comp", maxDegree, maxK, sdfFunction)
	{ }
};
#endif
