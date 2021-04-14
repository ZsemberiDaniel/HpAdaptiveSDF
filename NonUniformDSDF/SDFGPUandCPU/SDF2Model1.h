#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model1 : public SDFBase
{
public:
	SDF2Model1(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model1", "SDFGPUandCPU/SDF2Model1.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.5f, 0.0f, 0.5f);
		float r0 = sdfCylinderX(p - vec3(0.0f, 0.65f, 0.0f), vec2(0.2f, 0.5f));
		r0 -= 0.01f;//offset
		float r1 = sdfCylinderZ(p - vec3(0.0f, 0.65f, 0.0f), vec2(0.2f, 0.5f));
		r1 -= 0.01f;//offset
		r0 = min(r0, r1); //union 1
		float r2 = sdfSphere(p - vec3(0.0f, 0.65f, 0.0f), 0.44f);
		r0 = min(r0, r2); //union 2
		float r3 = sdfCylinderY(p - vec3(0.0f, 0.575f, 0.0f), vec2(0.2f, 0.575f));
		r3 -= 0.01f;//offset
		r0 = min(r0, r3); //union 3
		float r4 = sdfBox(p, vec3(0.35f, 0.075f, 0.35f));
		r4 -= 0.01f;//offset
		r0 = min(r0, r4); //union 4
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model1() final = default;

private:
};
#endif
