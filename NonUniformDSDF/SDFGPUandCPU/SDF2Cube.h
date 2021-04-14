#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"
#include "imgui/imgui.h"

class SDF2Cube : public SDFBase
{
private:
	glm::vec3 sdfCubePosition = glm::vec3(0.5f);
	glm::vec3 sdfCubeHalfSize = glm::vec3(0.25f);
public:
	SDF2Cube(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Cube", "SDFGPUandCPU/SDF2Cube.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		vec3 d = abs(p - sdfCubePosition) - sdfCubeHalfSize;
		return min(max(d.x, max(d.y, d.z)), 0.0f) + length(max(d, 0.0f));
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
		ImGui::DragFloat3("Position", &sdfCubePosition.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat3("Half size", &sdfCubeHalfSize.x, 0.01f, 0.01f, 1.0f);
	}

	~SDF2Cube() final = default;

protected:
	void writeAttributes(std::ofstream& out) const final
	{
		using namespace writeDefinesOperators;
		out << "#define sdfCubePosition " << sdfCubePosition << std::endl;
		out << "#define sdfCubeHalfSize " << sdfCubeHalfSize << std::endl;
	}
};
#endif