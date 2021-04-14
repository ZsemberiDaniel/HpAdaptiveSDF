#pragma once

#ifndef GPU_SIDE

#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"

class SDF2Torus : public SDFBase
{
public:
	SDF2Torus(glm::vec3 worldMinPos = glm::vec3(-1), float worldSize = 2.0f) :
		SDFBase("Torus", "SDFGPUandCPU/SDF2Torus.h", worldMinPos, worldSize) {}
	
	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= sdfTorusPosition;
		vec2 q = vec2(length(vec2(p.x, p.z)) - sdfTorusOuterRadius, p.y);
		return (length(q) - sdfTorusInnerRadius);
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
		ImGui::DragFloat("Inner radius", &sdfTorusInnerRadius, 0.001f, 0.05f, 0.6f);
		ImGui::DragFloat("Outer radius", &sdfTorusOuterRadius, 0.001f, 0.05f, 0.6f);
		ImGui::DragFloat3("Position", &sdfTorusPosition.x, 0.001f, 0.0f, 1.0f);
	}
	
	~SDF2Torus() final = default;

private:
	glm::vec3 sdfTorusPosition = glm::vec3(0);
	float sdfTorusOuterRadius = 0.8f;
	float sdfTorusInnerRadius = 0.4f;

protected:
	void writeAttributes(std::ofstream& out) const final
	{
		using namespace writeDefinesOperators;
		out << "#define sdfTorusPosition " << sdfTorusPosition << std::endl;
		out << "#define sdfTorusOuterRadius " << sdfTorusOuterRadius << std::endl;
		out << "#define sdfTorusInnerRadius " << sdfTorusInnerRadius << std::endl;
	}
};
#endif