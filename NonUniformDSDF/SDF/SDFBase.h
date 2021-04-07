#ifndef SDFBASE_H
#define SDFBASE_H
#include <glm/vec3.hpp>
#include "../vector3d.h"
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/buffer.h> //will be replaced
#include <Dragonfly/detail/Texture/Texture3D.h>
#include <Dragonfly/detail/Texture/Texture2DArray.h>
#include "../constants.h"

class SDFBase
{
private:
	float maxSDFValue = -std::numeric_limits<float>::max();

	bool discreteSDFInitialized = false;
	std::shared_ptr<df::Texture3D<float>> discreteSDFTexture = nullptr;
	std::shared_ptr<df::Texture2DArray<float>> discreteSDFTexture2D = nullptr;

protected:
	glm::vec3 _worldMinPos;
	float _worldSize;

	std::string _sdfName = "Default SDF name";
public:
	SDFBase(std::string sdfName, glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		_sdfName(sdfName), _worldMinPos(worldMinPos), _worldSize(worldSize) 
	{
		
	}
	virtual float operator()(glm::vec3 p) const { return 0.0f; }
	virtual void renderGUI()
	{
		ImGui::InputFloat3("World min pos", &_worldMinPos.x, 1);
		ImGui::InputFloat("World size", &_worldSize, 1);
	}
	virtual ~SDFBase() 
	{	
		
	}

	void initializeSDFDiscreteValues()
	{
		std::vector<float> temp(ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE);
		float heatMapSize = (float)ERROR_HEATMAP_SIZE;
		for (int x = 0; x < ERROR_HEATMAP_SIZE; x++)
		{
			for (int y = 0; y < ERROR_HEATMAP_SIZE; y++)
			{
				for (int z = 0; z < ERROR_HEATMAP_SIZE; z++)
				{
					float val = this->operator()(glm::vec3(_worldSize) * glm::vec3(x, y, z) / heatMapSize);
					temp[ERROR_HEATMAP_SIZE * ERROR_HEATMAP_SIZE * x + ERROR_HEATMAP_SIZE * y + z] = val;

					if (glm::abs(val) > maxSDFValue) maxSDFValue = glm::abs(val);
				}
			}
		}

		discreteSDFInitialized = true;

		discreteSDFTexture = std::make_shared<df::Texture3D<float>>(static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE));
		discreteSDFTexture->LoadData(temp);

		discreteSDFTexture2D = std::make_shared<df::Texture2DArray<float>>(static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE), static_cast<GLuint>(ERROR_HEATMAP_SIZE));
		discreteSDFTexture2D->LoadData(temp);
		GL_CHECK;
	}

	glm::vec3 worldMinPos() const { return _worldMinPos; }
	float worldSize() const { return _worldSize; }
	std::string name() const { return _sdfName; }

	std::shared_ptr<df::Texture3D<float>> discreteSDFValuesTexture()
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return discreteSDFTexture;
	}

	std::shared_ptr<df::Texture2DArray<float>> discreteSDFValuesTexture2D()
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return discreteSDFTexture2D;
	}

	float discreteMaxSDFValue() 
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return maxSDFValue;
	}
};
#endif