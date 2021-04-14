#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"

class SDF2Sphere : public SDFBase
{
public:
	SDF2Sphere(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Sphere", "SDFGPUandCPU/SDF2Sphere.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		return length(p - sdfSpherePosition) - sdfSphereRadius;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
		ImGui::DragFloat("Radius", &sdfSphereRadius, 0.001f, 0.05f, 0.6f);
		ImGui::DragFloat3("Position", &sdfSpherePosition.x, 0.001f, 0.0f, 1.0f);
	}

	~SDF2Sphere() final = default;

private:
	glm::vec3 sdfSpherePosition = glm::vec3(0.5f);
	float sdfSphereRadius = 0.4f;

protected:
	void writeAttributes(std::ofstream& out) const final
	{
		using namespace writeDefinesOperators;
		out << "#define sdfSpherePosition " << sdfSpherePosition << std::endl;
		out << "#define sdfSphereRadius " << sdfSphereRadius << std::endl;
	}
};
#endif
