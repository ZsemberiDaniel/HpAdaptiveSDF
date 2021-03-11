#ifndef SDFCUBE_H
#define SDFCUBE_H
#include <glm/glm.hpp>

#include "SDFBase.h"
#include "SDFPrimitives.h"
#include "imgui/imgui.h"

class SDFCube : public SDFBase
{
public:
	SDFCube(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Cube", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		glm::vec3 d = abs(p - position) - halfSize;
		return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.0f) + glm::length(glm::max(d, 0.0f));
	}

	void renderGUI() final
	{
		SDFBase::renderGUI();
		ImGui::DragFloat3("Position", &position.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat3("Half size", &halfSize.x, 0.01f, 0.01f, 1.0f);
	}

	~SDFCube() final = default;

private:
	glm::vec3 position = glm::vec3(0.5f);
	glm::vec3 halfSize = glm::vec3(0.25f);
};
#endif