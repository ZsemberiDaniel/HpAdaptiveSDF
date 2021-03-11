#ifndef SDFPLANE_H
#define SDFPLANE_H
#include <glm/glm.hpp>

#include "SDFBase.h"
#include "imgui/imgui.h"

class SDFPlane : public SDFBase
{
public:
	SDFPlane(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Plane", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		return dot(p - position, glm::normalize(normal));
	}

	void renderGUI() final
	{
		SDFBase::renderGUI();
		ImGui::DragFloat3("Position", &position.x, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat3("Normal", &normal.x, 0.001f, 0.0f, 1.0f);
	}

	~SDFPlane() final = default;

private:
	glm::vec3 position = glm::vec3(0, 0.5f, 0);
	glm::vec3 normal = glm::vec3(0, 1, 0);
};
#endif