#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"
#include "imgui/imgui.h"

class SDF2Plane : public SDFBase
{
public:
	SDF2Plane(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Plane", "SDFGPUandCPU/SDF2Plane.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		return dot(p - sdfPlanePosition, normalize(sdfPlaneNormal));
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
		ImGui::DragFloat3("Position", &sdfPlanePosition.x, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat3("Normal", &sdfPlaneNormal.x, 0.001f, 0.0f, 1.0f);
	}

	~SDF2Plane() final = default;

private:
	glm::vec3 sdfPlanePosition = glm::vec3(0, 0.5f, 0);
	glm::vec3 sdfPlaneNormal = glm::vec3(0, 1, 0);

protected:
	void writeAttributes(std::ofstream& out) const final
	{
		using namespace writeDefinesOperators;
		out << "#define sdfPlanePosition " << sdfPlanePosition << std::endl;
		out << "#define sdfCubeHalfSize " << sdfPlaneNormal << std::endl;
	}
};
#endif