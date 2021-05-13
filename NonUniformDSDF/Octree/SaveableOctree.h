#ifndef SAVEABLEOCTREE_H
#define SAVEABLEOCTREE_H
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <Dragonfly/detail/buffer.h>
#include <Dragonfly/detail/Texture/Texture2DArray.h>
#include <Dragonfly/detail/Texture/Texture3D.h>
#include "octree.h"
#include "octreeGenerator.h"
#include "../Math/PolynomialBases.h"
#include "../Structures.h"
#include "../SDF/SDFHeader.h"
#include <filesystem>

class SaveableOctree
{
private:
	SaveableOctree() { }

	std::string name;
	std::unique_ptr<Octree<Cell>> octree = nullptr;

	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> branchSSBO;
	std::unique_ptr<eltecg::ogl::ShaderStorageBuffer> leavesSSBO;

	std::unique_ptr<df::ComputeProgramEditor> texture2DArrayCalculator;

	struct LookupTable
	{
		std::shared_ptr<df::Texture3D<df::integral<uint32_t>>> texture;
		float lookupTableNormalizedCellSize;
		// how many cells per side
		int lookupTableDim1Size;
	} lookupTable;


	void initTexture2DArrayCalculator()
	{
		using namespace df;

		std::map<std::string, std::string> m;
		saveDefinesFile(m, "Shaders/tempDefinesSaveableOctreeTemp.glsl");

		std::cout << "Compiling SaveableOctree texture2DArray calculator" << std::endl;
		texture2DArrayCalculator = std::make_unique<df::ComputeProgramEditor>("Texture2DArray-prog");
		*texture2DArrayCalculator
			<< "Shaders/tempDefinesSaveableOctreeTemp.glsl"_comp
			<< "Shaders/uniforms.glsl"_comp
			<< "Shaders/Math/common.glsl"_comp
			<< "Shaders/common.glsl"_comp
			<< "Shaders/Octree/octree.glsl"_comp
			<< "Shaders/SDF/sdfOctree.glsl"_comp
			<< "Shaders/Evaluate/threeDimFromOctree.comp"_comp
			<< df::LinkProgram;
		std::cout << texture2DArrayCalculator->GetErrors();
		std::filesystem::remove("Shaders/tempDefinesSaveableOctreeTemp.glsl");
	}

	/// <summary>
	/// These need to be saved to a file
	/// </summary>
	struct FileData
	{
		struct SavedToBinary
		{
			float compressCoefficient;
			int branchCount;
			int sdfIndex;
			int approxTypeIndex;
			OctreeGenerator::ConstructionParameters constructionParams;
		} dat;

		// These also need to be saved to a file, but a vector cannot be stored as binary data
		std::vector<unsigned int> branchesGPU;
		std::vector<unsigned int> leavesGPU;
	} fileData;

	inline static std::shared_ptr<SaveableOctree> initFromFileData(FileData& data)
	{
		auto saveable = std::shared_ptr<SaveableOctree>(new SaveableOctree());

		saveable->fileData = data;
		saveable->octree = std::make_unique<Octree<Cell>>();

		// later used for subdivision
		Cell empty;
		vector3d<Cell> subdividedCell(2);
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int z = 0; z < 2; z++)
				{
					subdividedCell(x, y, z) = empty;
				}
			}
		}

		// initial bounding box
		std::dynamic_pointer_cast<Octree<Cell>::Leaf>(saveable->octree->root())->value().bbox = BoundingBox{
			saveable->getConstructionParams().minPos, saveable->getConstructionParams().minPos + saveable->getConstructionParams().sizeInWorld
		};

		std::queue<std::pair<int, std::shared_ptr<Octree<Cell>::Leaf>>> growingBranches;
		// root is stored at the end
		growingBranches.push(std::make_pair(saveable->branchCount() - 1, std::dynamic_pointer_cast<Octree<Cell>::Leaf>(saveable->octree->root())));

		// fill octree with gpu data
		const int BRANCH_SIZE = 9;
		while (!growingBranches.empty())
		{
			// pop next growing branch and give alias
			auto& pair = growingBranches.front();
			growingBranches.pop();

			int i = pair.first;
			std::shared_ptr<Octree<Cell>::Leaf> leaf = pair.second;

			// it is a branch
			if (i < saveable->branchCount())
			{
				// subdivide further and push to queue
				vector3d<std::shared_ptr<Octree<Cell>::Leaf>> leaves(2);
				leaf->subdivide(saveable->octree.get(), subdividedCell, leaves);

				glm::vec3 subdividedSize = leaf->value().bbox.size() / 2.0f;

				for (int x = 0; x < 2; x++)
				{
					for (int y = 0; y < 2; y++)
					{
						for (int z = 0; z < 2; z++)
						{
							int index = z * 4 + y * 2 + x;
							int childId = saveable->fileData.branchesGPU[BRANCH_SIZE * i + 1 + index];
							leaves(x, y, z)->value().bbox = BoundingBox{
								leaf->value().bbox.min + subdividedSize * glm::vec3(x, y, z),
								leaf->value().bbox.min + subdividedSize * (glm::vec3(x, y, z) + 1.0f)
							};
							growingBranches.push(std::make_pair(childId, leaves(x, y, z)));
						}
					}
				}
			}
			// it is a leaf
			else
			{
				int leafIndexInSSBO = i - saveable->branchCount();
				leaf->value().octreeLeaf = leaf;

				glm::vec3 readPos;
				unsigned int level = saveable->fileData.leavesGPU[leafIndexInSSBO + 0];
				readPos.x = glm::uintBitsToFloat(saveable->fileData.leavesGPU[leafIndexInSSBO + 1]);
				readPos.y = glm::uintBitsToFloat(saveable->fileData.leavesGPU[leafIndexInSSBO + 2]);
				readPos.z = glm::uintBitsToFloat(saveable->fileData.leavesGPU[leafIndexInSSBO + 3]);

				assert(leaf->value().level() == level);

				int degree = saveable->fileData.leavesGPU[leafIndexInSSBO + 4];
				Polynomial poly(degree);

				for (int i = 0; i < poly.coeffCount(); i += 2)
				{
					glm::vec2 coeffs = glm::unpackSnorm2x16(saveable->fileData.leavesGPU[leafIndexInSSBO + 5 + i / 2]);
					poly[i + 0] = coeffs.x * saveable->compressCoefficient();
					if (i + 1 < poly.coeffCount()) poly[i + 1] = coeffs.y * saveable->compressCoefficient();
				}

				leaf->value().poly = poly;
			}
		}

		saveable->initClassFromFileData();
		return saveable;
	}

	/// <summary>
	/// Should be called after fileData is complete. Inits other members.
	/// </summary>
	void initClassFromFileData()
	{
		branchSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
		branchSSBO->constructImmutable(fileData.branchesGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);

		leavesSSBO = std::make_unique<eltecg::ogl::ShaderStorageBuffer>();
		leavesSSBO->constructImmutable(fileData.leavesGPU, eltecg::ogl::BufferFlags::MAP_READ_BIT);

		std::stringstream ss;
		ss << sdfFunction()->name() << approxType().shortUniqueName << "D" << getConstructionParams().maxDegree << "L" << getConstructionParams().maxLevel;
		if (getConstructionParams().useHAdapt && !getConstructionParams().usePAdapt) ss << "H";
		if (getConstructionParams().usePAdapt && !getConstructionParams().useHAdapt) ss << "P";

		name = ss.str();
	}

	/// <summary>
	/// Returns the Cell with the given position. The input should be in [0;1]^3
	/// </summary>
	const Cell& getValue(glm::vec3 pos) const
	{
		static GLuint twoPow[25] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097153, 4194304, 8388608, 16777216 };

		glm::vec3 localP = pos;
		// last element is the root
		std::shared_ptr<Octree<Cell>::Node> currentNode = octree->root();

		glm::vec3 boxMin(0.0f);
		glm::vec3 boxMax(1.0f);

		while (currentNode->type() == Octree<Cell>::BranchNode)
		{
			glm::vec3 halfBoxSize(1.0f / twoPow[currentNode->layer()]);
			// int pointerId = int(localP.z >= 0.5f) * 4 + int(localP.y >= 0.5f) * 2 + int(localP.x >= 0.5f);
			int pointerId = 0;
			if (localP.z > 0.5f)
			{
				localP.z -= 0.5f;
				pointerId += 4;

				boxMin.z += halfBoxSize.z;
			}
			else
			{
				boxMax.z -= halfBoxSize.z;
			}

			if (localP.y > 0.5f)
			{
				localP.y -= 0.5f;
				pointerId += 2;

				boxMin.y += halfBoxSize.y;
			}
			else
			{
				boxMax.y -= halfBoxSize.y;
			}

			if (localP.x > 0.5f)
			{
				localP.x -= 0.5f;
				pointerId += 1;

				boxMin.x += halfBoxSize.x;
			}
			else
			{
				boxMax.x -= halfBoxSize.x;
			}


			localP *= 2.0f;
			currentNode = std::static_pointer_cast<Octree<Cell>::Branch>(currentNode)->child(pointerId);
		}

		return std::static_pointer_cast<Octree<Cell>::Leaf>(currentNode)->value();
	}

public:

	SaveableOctree(int sdfIndex, int approxIndex, OctreeGenerator::ConstructionParameters& constructionParams);

	/// <summary>
	/// Which SDF function was used to construct this octree
	/// </summary>
	SDFBase* sdfFunction() { return sdfs[sdfIndex()]; }
	/// <summary>
	/// Which approximation type was used to construct this octree
	/// </summary>
	PolynomialBase& approxType() { return approxTypes[approxTypeIndex()]; }
	/// <summary>
	/// Which approximation type was used to construct this octree
	/// </summary>
	const PolynomialBase& approxType() const { return approxTypes[approxTypeIndex()]; }

	/// <summary>
	/// Prints the octree to the console.
	/// </summary>
	void printOctree()
	{
		if (octree == nullptr) return;

		octree->print(getConstructionParams().minPos, getConstructionParams().minPos + getConstructionParams().sizeInWorld);
	}

	void saveTo(std::wstring path)
	{
		std::ofstream file(path, std::ios::out | std::ios::binary);

		// write class itself
		size_t size = sizeof(FileData::SavedToBinary);
		file.write((char*)&size, sizeof(size_t));
		file.write((char*)&fileData, size);

		// write std vector data
		size = fileData.branchesGPU.size();
		file.write((char*)&size, sizeof(size_t));
		file.write((char*)&fileData.branchesGPU[0], size * sizeof(unsigned int));

		size = fileData.leavesGPU.size();
		file.write((char*)&size, sizeof(size_t));
		file.write((char*)&fileData.leavesGPU[0], size * sizeof(unsigned int));

		file.close();
	}

	inline static std::shared_ptr<SaveableOctree> loadFrom(std::wstring path)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary);

		FileData data;

		size_t size;
		// read class itself
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		file.read(reinterpret_cast<char*>(&data.dat), size);

		// read vectors
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		data.branchesGPU.resize(size);
		file.read(reinterpret_cast<char*>(&data.branchesGPU[0]), size * sizeof(unsigned int));

		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		data.leavesGPU.resize(size);
		file.read(reinterpret_cast<char*>(&data.leavesGPU[0]), size * sizeof(unsigned int));

		file.close();

		auto out = initFromFileData(data);
		out->initTexture2DArrayCalculator();
		return out;
	}

	void renderConstructionParamsGUI()
	{
		ImGui::Text("Parameters:");
		ImGui::Text("SDF function: %s", sdfFunction()->name().c_str());
		ImGui::Text("Approximation: %s", approxType().name.c_str());
		ImGui::Text("Maximum degree: %d", getConstructionParams().maxDegree);
		ImGui::Text("Maximum level: %d", getConstructionParams().maxLevel);
		ImGui::Text("Error threshold: %.9f", getConstructionParams().errorThreshold);
		ImGui::Text("Storage size: %d byte", sizeof(GLuint) * (fileData.branchesGPU.size() + fileData.leavesGPU.size()));
		
		if (getConstructionParams().useHAdapt && getConstructionParams().usePAdapt)
		{
			ImGui::Text("Both P and H adapt were used.");
		} 
		else if (!getConstructionParams().useHAdapt && getConstructionParams().usePAdapt)
		{
			ImGui::Text("Used only P adapt.");
		}
		else if (getConstructionParams().useHAdapt && !getConstructionParams().usePAdapt)
		{
			ImGui::Text("Used only H adapt.");
		}
	}

	/* Writes the given values in the map to a glsl file as define directives, the key of the map being
	the identifier and the value being the token-string. Also writes additional directives for this octree.
	*/
	void saveDefinesFile(std::map<std::string, std::string> additionalDefines, std::string path = "Shaders/defines.glsl")
	{
		std::map<std::string, std::string> defines{
			// which type of evalutaion to use for the polynomials
			{ "evalPolynom", approxType().shaderEvalFunctionName },

			{ "MAX_DEGREE", std::to_string(getConstructionParams().maxDegree) },
			{ "MAX_COEFF_SIZE", std::to_string(Polynomial::calculateCoeffCount(getConstructionParams().maxDegree)) },
			// used for the SDFs that work for both CPU and GPU
			{ "GPU_SIDE", "" }
		};
		for (auto& pair : additionalDefines)
		{
			defines[pair.first] = pair.second;
		}

		std::ofstream out(path);
		if (!out.is_open())
		{
			std::cout << "Could not open defines file!" << std::endl;
		}
		for (auto const& define : defines)
		{
			out << "#define " << define.first << " " << define.second << "\n";
		}

		out.close();
	}

	LookupTable getLookupTable()
	{
		if (lookupTable.texture != nullptr) return lookupTable;

		// generate lookup table
		lookupTable.texture = std::make_shared<df::Texture3D<df::integral<uint32_t>>>();

		std::vector<df::integral<uint32_t>> pointers;
		int lookupTableDim1Size = 0;
		octree->generate3DLookupTable(pointers, lookupTableDim1Size);

		// set values
		lookupTable.texture->InitTexture(lookupTableDim1Size, lookupTableDim1Size, lookupTableDim1Size, 1);
		lookupTable.texture->LoadData(pointers, false);
		lookupTable.lookupTableDim1Size = lookupTableDim1Size;
		lookupTable.lookupTableNormalizedCellSize = 1.0f / lookupTableDim1Size;

		return lookupTable;
	}

	template<typename S, typename U>
	void SetOctreeUniforms(df::ProgramEditor<S, U>& program);

	/// <summary>
	/// Called from the approximator objects to generate this octree. SHOULD NOT BE USED OTHERWISE
	/// </summary>
	template<typename generator>
	void constructOctree(generator& gen);

	const std::unique_ptr<Octree<Cell>>& getOctree() { return octree; }
	const OctreeGenerator::ConstructionParameters& getConstructionParams() { return fileData.dat.constructionParams; }

	float compressCoefficient() const { return fileData.dat.compressCoefficient; }
	int branchCount() const { return fileData.dat.branchCount; }
	int sdfIndex() const { return fileData.dat.sdfIndex; }
	int approxTypeIndex() const { return fileData.dat.approxTypeIndex; }

	const std::string& getName() { return name; }

private:
	void calculateSdf0thOrderTempVector(int size, std::vector<float>& sdfValues) const
	{
		sdfValues.clear();
		sdfValues.resize(size * size * size);

		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				for (int z = 0; z < size; z++)
				{
					glm::vec3 p = glm::vec3(x, y, z) / (size - 1.0f);
					const Cell& cell = getValue(p);
					glm::vec3 posInCell = (p - cell.bbox.min) / cell.bbox.size();

					sdfValues[x * size * size + y * size + z] = cell.poly(posInCell * 2.0f - 1.0f, approxType().resultingPolynomialType, cell.bbox.size());
				}
			}
		}
	}

public:
	std::shared_ptr<df::Texture3D<float>> calculateSdf0thOrderTexture(int size) const
	{
		std::shared_ptr<df::Texture3D<float>> out = std::make_shared<df::Texture3D<float>>(size, size, size);
		std::vector<float> sdfValues;

		calculateSdf0thOrderTempVector(size, sdfValues);
		
		out->LoadData(sdfValues);

		return out;
	}

	std::shared_ptr<df::Texture2DArray<float>> calculateSdf0thOrderTexture2D(int size)
	{
		std::shared_ptr<df::Texture2DArray<float>> out = std::make_shared<df::Texture2DArray<float>>(size, size, size);
		glBindImageTexture(6, out->operator GLuint(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

		SetOctreeUniforms(*texture2DArrayCalculator);

		glDispatchCompute(size, size, size);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

		return out;
	}
};


template<typename S, typename U>
void SaveableOctree::SetOctreeUniforms(df::ProgramEditor<S, U>& program)
{
	branchSSBO->bindBufferRange(0);
	leavesSSBO->bindBufferRange(1);
	getLookupTable().texture->bind(2);

	program << "sdfTexSize" << glm::vec3(getConstructionParams().sizeInWorld)
		<< "sdfTexCorner" << getConstructionParams().minPos 
		<< "modelScale" << glm::vec3(getConstructionParams().sizeInWorld)
		<< "modelTrans" << getConstructionParams().minPos
		//<< "sTranslation" << glm::vec3(0) // state.octreeConstructionParams.minPos
		//<< "sScale" << glm::vec3(1)
		// << "octreeSize" << state.octreeConstructionParams.sizeInWorld
		<< "coeffCompressAmount" << compressCoefficient()
		<< "branchCount" << branchCount()
		<< "lookupTableSize" << lookupTable.lookupTableDim1Size;
}

template<typename generator>
void SaveableOctree::constructOctree(generator& gen)
{
	OctreeGenerator::constructField<generator>(
		octree,
		gen,
		fileData.dat.constructionParams,
		sdfFunction());
}
#endif