#ifndef SAVEABLEOCTREE_H
#define SAVEABLEOCTREE_H
#include <vector>

#include <Dragonfly/detail/buffer.h>
#include "octree.h"
#include "octreeGenerator.h"
#include "../Math/PolynomialBases.h"
#include "../Structures.h"
#include "../SDF/SDFHeader.h"

class SaveableOctree
{
public:

	SaveableOctree(int sdfIndex, int approxIndex, OctreeGenerator::ConstructionParameters& constructionParams);

	/// <summary>
	/// Which SDF function was used to construct this octree
	/// </summary>
	SDFBase* sdfFunction() { return sdfs[sdfIndex]; }
	/// <summary>
	/// Which approximation type was used to construct this octree
	/// </summary>
	PolynomialBase& approxType() { return approxTypes[approxTypeIndex]; }

	/// <summary>
	/// Prints the octree to the console.
	/// </summary>
	void printOctree()
	{
		if (octree == nullptr) return;

		octree->print(constructionParams.minPos, constructionParams.minPos + constructionParams.sizeInWorld);
	}

	template<typename S, typename U>
	void SetOctreeUniforms(df::ProgramEditor<S, U>& program);

	/// <summary>
	/// Called from the approximator objects to generate this octree. SHOULD NOT BE USED OTHERWISE
	/// </summary>
	template<typename generator>
	void constructOctree(generator& gen);

	const std::unique_ptr<Octree<Cell>>& getOctree() { return octree; }
	const OctreeGenerator::ConstructionParameters& getConstructionParams() { return constructionParams; }
	
private:
	float compressCoefficient;
	int branchCount;
	int sdfIndex;
	int approxTypeIndex;
	OctreeGenerator::ConstructionParameters constructionParams;
	std::unique_ptr<Octree<Cell>> octree = nullptr;
	
	std::vector<unsigned int> branchesGPU;
	std::vector<unsigned int> leavesGPU;
	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> branchSSBO;
	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> leavesSSBO;
};


template<typename S, typename U>
void SaveableOctree::SetOctreeUniforms(df::ProgramEditor<S, U>& program)
{
	branchSSBO->bindBufferRange(0);
	leavesSSBO->bindBufferRange(1);

	program << "sdfTexSize" << glm::vec3(1.0f) // glm::vec3(state.octreeConstructionParams.sizeInWorld)
		<< "sdfTexCorner" << glm::vec3(0.0f) // state.octreeConstructionParams.minPos
		<< "sTranslation" << glm::vec3(0) // state.octreeConstructionParams.minPos
		<< "sScale" << 1.0f // state.octreeConstructionParams.sizeInWorld
		// << "octreeSize" << state.octreeConstructionParams.sizeInWorld
		<< "coeffCompressAmount" << compressCoefficient
		<< "branchCount" << branchCount;
}

template<typename generator>
void SaveableOctree::constructOctree(generator& gen)
{
	OctreeGenerator::constructField<generator>(
		octree,
		gen,
		constructionParams,
		sdfFunction());
}
#endif