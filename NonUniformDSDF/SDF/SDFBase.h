#ifndef SDFBASE_H
#define SDFBASE_H
#include <glm/vec3.hpp>
#include "../vector3d.h"
#include <Dragonfly/editor.h>		 //inlcludes most features
#include <Dragonfly/detail/buffer.h> //will be replaced
#include "../constants.h"

class SDFBase
{
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
		float heatMapSize = (float)ERROR_HEATMAP_SIZE;
		for (int x = 0; x < ERROR_HEATMAP_SIZE; x++)
		{
			for (int y = 0; y < ERROR_HEATMAP_SIZE; y++)
			{
				for (int z = 0; z < ERROR_HEATMAP_SIZE; z++)
				{
					sdfValues->get(x, y, z) = this->operator()(glm::vec3(_worldSize) * glm::vec3(x, y, z) / heatMapSize);

					if (glm::abs(sdfValues->get(x, y, z)) > maxSDFValue) maxSDFValue = glm::abs(sdfValues->get(x, y, z));
				}
			}
		}

		discreteSDFInitialized = true;
	}

	glm::vec3 worldMinPos() const { return _worldMinPos; }
	float worldSize() const { return _worldSize; }
	std::string name() const { return _sdfName; }

	std::shared_ptr<vector3d<float>> discreteSDFValues()
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return sdfValues;
	}

	float discreteMaxSDFValue() 
	{
		if (!discreteSDFInitialized)
		{
			initializeSDFDiscreteValues();
		}
		return maxSDFValue;
	}

private:
	std::shared_ptr<vector3d<float>> sdfValues = std::make_shared<vector3d<float>>(ERROR_HEATMAP_SIZE, 0.0f);
	float maxSDFValue = -std::numeric_limits<float>::max();

	bool discreteSDFInitialized = false;


protected:
	glm::vec3 _worldMinPos;
	float _worldSize;

	std::string _sdfName = "Default SDF name";
};
#endif