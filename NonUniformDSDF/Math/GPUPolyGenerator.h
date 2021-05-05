#ifndef GPUPOLYNOMIALGENERATOR_H
#define GPUPOLYNOMIALGENERATOR_H

#include <filesystem>
#include "PolynomialGenerator.h"
#include "../Structures.h"
#include "IntegralEvaluator.h"
#include <glm/glm.hpp>
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/Events/renderdoc_load_api.h>

template<typename sdf>
class GPUPolyGenerator : public PolynomialGenerator
{
protected:
	std::vector<int> ssbosPerDegreesSize;
	GLuint ssboShaderCompute, ssboPolyError, ssboPolyDescription;
	df::ComputeProgramEditor compShader = df::ComputeProgramEditor("Polynomial compute shader");

	virtual void addWhileShaderCompile() { }
	virtual void beforeCompShaderRun() { }

public:
	GPUPolyGenerator(std::string pathToPolyGenShader, int maxDegree, int maxK, sdf& sdfFunction)
	{
		// write sdf func defines
		sdfFunction.writeAttributesToDefinesFile("Shaders/octreeGenerateDefines.glsl");

		// write other attribute defines
		std::ofstream out("Shaders/gpuGenDefines.glsl");
		if (!out.is_open())
		{
			std::cout << "Could not open gpu generator defines file!" << std::endl;
		}
		out << "#define GPU_SIDE " << std::endl;
		out << "#define MAX_COEFF_SIZE " << std::to_string(Polynomial::calculateCoeffCount(maxDegree)) << std::endl;
		out << "#define MAX_DEGREE " << std::to_string(maxDegree) << std::endl;
		out << "#define evalPolynom evalPolynom_lagrange" << std::endl;
		out.close();

		compShader
			<< "Shaders/gpuGenDefines.glsl"_comp
			<< "Shaders/octreeGenerateDefines.glsl"_comp
			<< "Shaders/Math/common.glsl"_comp
			<< "Shaders/PolyGen/sdfPrimitives.comp"_comp;
		addWhileShaderCompile();
		compShader
			<< df::detail::_CompShader{ sdfFunction.path().c_str() }
			<< df::detail::_CompShader{ pathToPolyGenShader.c_str() }
			<< "Shaders/PolyGen/PolyGen.comp"_comp
			<< df::LinkProgram;
		std::cout << compShader.GetErrors();
		std::filesystem::remove("Shaders/gpuGenDefines.glsl");

		for (int i = 0; i <= maxDegree; i++)
		{
			ssbosPerDegreesSize.push_back(Polynomial::calculateCoeffCount(i + 1) + 8 * Polynomial::calculateCoeffCount(i));
		}
		glGenBuffers(1, &ssboShaderCompute);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboShaderCompute);
		std::vector<float> container(maxK * ssbosPerDegreesSize.back(), 0.0f);
		int bufferSize = static_cast<int>(container.size() * sizeof(float));
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)container.data(), GL_MAP_WRITE_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


		glGenBuffers(1, &ssboPolyError);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPolyError);
		container = std::vector<float>(maxK * 9, 0.0f);
		bufferSize = static_cast<int>(container.size() * sizeof(float));
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)container.data(), GL_MAP_WRITE_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


		glGenBuffers(1, &ssboPolyDescription);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPolyDescription);
		std::vector<glm::vec4> container2 = std::vector<glm::vec4>(maxK * 2, glm::vec4(2.0f));
		bufferSize = static_cast<int>(container2.size() * sizeof(glm::vec4));
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize, (GLvoid*)container2.data(), GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	virtual ~GPUPolyGenerator()
	{
		glDeleteBuffers(1, &ssboShaderCompute);
		glDeleteBuffers(1, &ssboPolyError);
		glDeleteBuffers(1, &ssboPolyDescription);
	}

	std::vector<Polynomial> fitPolynomials(std::vector<std::pair<BoundingBox, int>>& bboxes, sdf& sdfFunction);
};


template<typename sdf>
std::vector<Polynomial> GPUPolyGenerator<sdf>::fitPolynomials(std::vector<std::pair<BoundingBox, int>>& data, sdf& sdfFunction)
{
	std::vector<Polynomial> polys(data.size() * 9);

	// size of the ssbo returned from the worker compute shaders
		// essentially the sum of the K cells
	int returnedSSBOSize = 0,
		cellGroupSize = 0; // the size of the current cell group
	std::vector<Cell> currentCellGroup(data.size());
	std::vector<glm::vec4> polyDesc(2 * data.size());
	
	int polyIndex = 0;
	for (cellGroupSize = 0; cellGroupSize < data.size(); cellGroupSize++, polyIndex++)
	{
		if (data[polyIndex].second < 0)
		{
			std::cerr << "COULD NOT FIT POLYNOMIAL WITH DEG " << data[polyIndex].second << std::endl;
			return polys;
		}

		polyDesc[2 * polyIndex + 0] = glm::vec4(data[polyIndex].first.min.x, data[polyIndex].first.min.y,
			data[polyIndex].first.min.z, glm::intBitsToFloat(data[polyIndex].second));
		polyDesc[2 * polyIndex + 1] = glm::vec4(data[polyIndex].first.max.x, data[polyIndex].first.max.y,
			data[polyIndex].first.max.z, glm::intBitsToFloat(returnedSSBOSize));

		returnedSSBOSize += ssbosPerDegreesSize[data[polyIndex].second];
	}

	// rdoc::API->StartFrameCapture(NULL, NULL);
	// running comp shaders
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPolyDescription);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, polyDesc.size() * sizeof(glm::vec4), (GLvoid*)polyDesc.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboShaderCompute);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboPolyError);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboPolyDescription);
	beforeCompShaderRun();
	compShader << "a" << 0;
	glDispatchCompute(1, cellGroupSize, 9);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	// rdoc::API->EndFrameCapture(NULL, NULL);

	/*if (rdoc::API != nullptr && !rdoc::API->IsTargetControlConnected())
		rdoc::API->LaunchReplayUI(1, nullptr);*/

	// getting data from GPU
	std::vector<float> polyStorage(returnedSSBOSize), errorStorage(9 * data.size());
	glGetNamedBufferSubData(ssboShaderCompute, 0, polyStorage.size() * sizeof(float), polyStorage.data());
	glGetNamedBufferSubData(ssboPolyError, 0, errorStorage.size() * sizeof(float), errorStorage.data());


	// saving it as CPU polynomial data
	int indexInPolyStorage = 0;
	for (int k = 0; k < data.size(); k++)
	{
		for (int i = 0; i < 9; i++)
		{
			// first is the p improvement result, rest 8 is the h improvement
			polys[9 * k + i] = Polynomial(data[k].second + (i == 0 ? 1 : 0));

			for (int j = 0; j < polys[9 * k + i].coeffCount(); j++)
			{
				polys[9 * k + i][j] = polyStorage[indexInPolyStorage++];
			}
			// float currentError = errorStorage[cellIdInGroup * 9 + m + 1];
		}
	}

	return polys;
}
#endif
