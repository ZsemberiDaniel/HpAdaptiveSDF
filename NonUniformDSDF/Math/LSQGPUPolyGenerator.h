#ifndef LSQGPUPOLYNOMIALGENERATOR_H
#define LSQSGPUPOLYNOMIALGENERATOR_H

#include "GPUPolyGenerator.h"
#include "../Eigen/Dense"

template<typename sdf>
class LSQGPUPolyGenerator : public GPUPolyGenerator<sdf>
{
private:
	std::vector<glm::vec4> points;
	
	GLuint ssboPseudoInverses;
	GLuint ssboPoints;

protected:
	void beforeCompShaderRun() override
	{ 
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboPseudoInverses);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboPoints);
	}

public:
	virtual ~LSQGPUPolyGenerator()
	{
		glDeleteBuffers(1, &ssboPseudoInverses);
		glDeleteBuffers(1, &ssboPoints);
	}

	LSQGPUPolyGenerator(int maxDegree, int maxK, sdf& sdfFunction) :
		GPUPolyGenerator<sdf>("Shaders/PolyGen/LSQPolyGen.comp", maxDegree, maxK, sdfFunction)
	{
		// if changed, LSQPolyGen.comp needs to be changed as well
		const int pointCount = 5;
		std::vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>> pseudoInverseMatrices;
		points.reserve(pointCount * pointCount * pointCount);

		// populate linearly interpolated points to vector
		for (int i = 0; i < pointCount; i++)
		{
			for (int j = 0; j < pointCount; j++)
			{
				for (int k = 0; k < pointCount; k++)
				{
					glm::vec3 p = glm::vec3(i, j, k) / (pointCount - 1.0f) * 2.0f - 1.0f;
					points.emplace_back(glm::vec4(p.x, p.y, p.z, 0.0f));
				}
			}
		}

		calculatePseudoInverseMatrices(pseudoInverseMatrices, maxDegree);
		/*std::vector<std::vector<std::vector<float>>> passToSSBO(maxDegree + 1);
		for (int deg = 0; deg <= maxDegree; deg++)
		{
			passToSSBO[deg].resize(pseudoInverseMatrices.back().rows());
			for (int coeff = 0; coeff < pseudoInverseMatrices.back().rows(); coeff++)
			{
				passToSSBO[deg][coeff].resize(points.size(), 0.0f);
			}
		}*/
		int maxCoeffSize = pseudoInverseMatrices.back().rows();
		std::vector<float> passToSSBO((maxDegree + 1) * maxCoeffSize * points.size());

		// each deg till maxDegree
		for (int deg = 0; deg <= maxDegree; deg++)
		{
			// each coeff in the polynomial
			for (int i = 0, m = 0; i <= deg; i++)
			{
				for (int k = 0; i + k <= deg; k++)
				{
					for (int j = 0; i + k + j <= deg; j++, m++)
					{
						// each point in LSQ
						for (int pointId = 0; pointId < points.size(); pointId++)
						{
							// passToSSBO[deg][m][pointId] = pseudoInverseMatrices[deg](m, pointId);
							passToSSBO[deg * maxCoeffSize * points.size() +
									   m * points.size() +
								       pointId] = pseudoInverseMatrices[deg](m, pointId);
						}
					}
				}
			}
		}
		// size_t passToSSBOSize = passToSSBO.size() * passToSSBO[0].size() * passToSSBO[0][0].size() * sizeof(float);

		glGenBuffers(1, &ssboPseudoInverses);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPseudoInverses);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, passToSSBO.size() * sizeof(float), (void*)passToSSBO.data(), GL_MAP_READ_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		GL_CHECK;


		glGenBuffers(1, &ssboPoints);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoints);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * points.size(), (void*)points.data(), GL_MAP_READ_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		GL_CHECK;
	}

private:
	void calculatePseudoInverseMatrices(std::vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>>& pseudoInverseMatrices, int maxDegree)
	{
		pseudoInverseMatrices.reserve(maxDegree + 1);
		for (int degree = 0; degree <= maxDegree; degree++)
		{
			pseudoInverseMatrices.emplace_back(Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>(Polynomial::calculateCoeffCount(degree), points.size()));
			auto& mat = pseudoInverseMatrices.back();

			std::vector<glm::vec3> base(degree + 1);
			int sid = 0;
			for (auto& samp : points)
			{
				// Legendre base functions
				// a = 1;
				// b = p;
				// float c = (1.0f / at) * ((2.0f * at - 1.0f) * p * b - (at - 1.0f) * a);

				base[0] = glm::vec3(1.0f);
				if (degree > 0) base[1] = glm::vec3(samp.x, samp.y, samp.z);
				for (int i = 2; i <= degree; ++i)
				{
					base[i] = (1.0f / i) * ((2.0f * i - 1.0f) * glm::vec3(samp.x, samp.y, samp.z) * base[i - 1] - (i - 1.0f) * base[i - 2]);
				}

				for (int i = 0, m = 0; i <= degree; i++)
				{
					for (int k = 0; i + k <= degree; k++)
					{
						for (int j = 0; i + k + j <= degree; j++, m++)
						{
							mat(m, sid) = base[i].x * base[k].y * base[j].z;
						}
					}
				}
				sid++;
			}

			mat = ((mat * mat.transpose()).inverse() * mat).eval();
		}
	}
};
#endif
