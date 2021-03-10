#pragma once
#include <glm/glm.hpp>

#include "SDFBase.h"

class SDFSphere : public SDFBase
{
public:
	SDFSphere(glm::vec3 worldMinPos = glm::vec3(0), glm::vec3 worldSize = glm::vec3(1)) :
		SDFBase("Sphere", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		return glm::length(p - position) - r;
	}

	void renderGUI() final
	{
		ImGui::DragFloat("Radius", &r, 0.001f, 0.05f, 0.6f);
		ImGui::DragFloat3("Position", &position.x, 0.001f, 0.0f, 1.0f);
	}

	~SDFSphere() final = default;

private:
	glm::vec3 position = glm::vec3(0.5f);
	float r = 0.4f;
};
