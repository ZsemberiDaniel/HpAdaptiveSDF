#include "SaveableOctree.h"

SaveableOctree::SaveableOctree(int sdfIndex, int approxIndex, OctreeGenerator::ConstructionParameters& constructionParams) :
	sdfIndex(sdfIndex), approxTypeIndex(approxIndex), constructionParams(constructionParams)
{
	this->constructionParams.minPos = sdfFunction()->worldMinPos();
	this->constructionParams.sizeInWorld = sdfFunction()->worldSize();
	approxType().cpuConstruction(this->octree, this->constructionParams, this->sdfFunction());

	octree->packForGPU(branchesGPU, leavesGPU, branchCount, compressCoefficient);

	branchSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
	branchSSBO->constructImmutable(branchesGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);

	leavesSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
	leavesSSBO->constructImmutable(leavesGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);
}
