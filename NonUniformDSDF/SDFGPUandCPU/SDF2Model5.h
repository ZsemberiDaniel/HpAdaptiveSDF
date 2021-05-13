#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model5 : public SDFBase
{
public:
	SDF2Model5(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model5", "SDFGPUandCPU/SDF2Model5.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.5f, 0.5f, 0.5f); 
		float r0 = sdfBox(p - vec3(0, -0.405f, 0), vec3(0.45f, 0.045f, 0.45f));
		float r1 = sdfBox(p, vec3(0.45f, 0.45f, 0.45f));
		r1 -= 0.045f;//offset
		float r2 = sdfBox(p, vec3(0.405f, 0.9f, 0.405f));
		r2 *= -1.f; //invert
		r1 = max(r1, r2); //intersect 1
		r0 = min(r0, r1); //union 1
		float r3 = sdfCylinderX(p - vec3(0, -0.225f, 0), vec2(0.1f, 1.08));
		r3 *= -1.f; //invert
		r0 = max(r0, r3); //intersect 1
		float r4 = sdfCylinderZ(p - vec3(0, -0.225f, 0), vec2(0.1f, 1.08));
		r4 *= -1.f; //invert
		r0 = max(r0, r4); //intersect 1
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model5() final = default;

private:
};
#endif
