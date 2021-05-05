#include "SaveableOctree.h"

SaveableOctree::SaveableOctree(int sdfIndex_, int approxIndex_, OctreeGenerator::ConstructionParameters& constructionParams_)
{
	fileData.dat.sdfIndex = sdfIndex_;
	fileData.dat.approxTypeIndex = approxIndex_;
	fileData.dat.constructionParams = constructionParams_;

	fileData.dat.constructionParams.minPos = sdfFunction()->worldMinPos();
	fileData.dat.constructionParams.sizeInWorld = sdfFunction()->worldSize();
	approxType().cpuConstruction(this->octree, fileData.dat.constructionParams, this->sdfFunction());

	octree->packForGPU(fileData.branchesGPU, fileData.leavesGPU, fileData.dat.branchCount, fileData.dat.compressCoefficient);

	initClassFromFileData();
	initTexture2DArrayCalculator();
}
