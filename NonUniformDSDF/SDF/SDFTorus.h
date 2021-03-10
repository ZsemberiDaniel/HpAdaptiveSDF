#pragma once
#include <glm/glm.hpp>

#include "SDFBase.h"

class SDFTorus : public SDFBase
{
public:
	SDFTorus(glm::vec3 worldMinPos = glm::vec3(0), glm::vec3 worldSize = glm::vec3(1)) :
		SDFBase("Torus", worldMinPos, worldSize) {}
	
	float operator()(glm::vec3 p) const final
	{
		// p -= glm::vec3(1);
		glm::vec2 q = glm::vec2(glm::length(glm::vec2(p.x, p.z)) - R, p.y);
		return length(q) - r;
	}

	void renderGUI() final
	{
		ImGui::DragFloat("Inner radius", &r, 0.001f, 0.05f, 0.6f);
		ImGui::DragFloat("Outer radius", &R, 0.001f, 0.05f, 0.6f);
		ImGui::DragFloat3("Position", &position.x, 0.001f, 0.0f, 1.0f);
	}
	
	~SDFTorus() final = default;

private:
	glm::vec3 position = glm::vec3(0);
	float R = 0.8f;
	float r = 0.4f;
};
