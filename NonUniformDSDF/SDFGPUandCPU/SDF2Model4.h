#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model4 : public SDFBase
{
public:
	SDF2Model4(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model4", "SDFGPUandCPU/SDF2Model4.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.5f, 0.5f, 0.5f); 
		float r0 = sdfBox(p, vec3(0.45f, 0.135f, 0.45f));
		float r1 = sdfCylinderY(p - vec3(-0.45f, 0, -0.45f), vec2(0.18f, 0.27f));
		r1 *= -1.f; //invert
		r0 = max(r0, r1); //intersect 1
		float r2 = sdfCylinderY(p - vec3(0.45f, 0, -0.45f), vec2(0.18f, 0.27f));
		r2 *= -1.f; //invert
		r0 = max(r0, r2); //intersect 1
		float r3 = sdfCylinderY(p - vec3(-0.45f, 0, 0.45f), vec2(0.18f, 0.27f));
		r3 *= -1.f; //invert
		r0 = max(r0, r3); //intersect 1
		float r4 = sdfCylinderY(p - vec3(0.45f, 0, 0.45f), vec2(0.18f, 0.27f));
		r4 *= -1.f; //invert
		r0 = max(r0, r4); //intersect 1
		float r5 = sdfCylinderY(p - vec3(-0.198f, 0, -0.378f), vec2(0.036f, 0.27f));
		r5 *= -1.f; //invert
		r0 = max(r0, r5); //intersect 1
		float r6 = sdfCylinderY(p - vec3(0.378f, 0, -0.198f), vec2(0.036f, 0.27f));
		r6 *= -1.f; //invert
		r0 = max(r0, r6); //intersect 1
		float r7 = sdfCylinderY(p - vec3(-0.378f, 0, 0.198f), vec2(0.036f, 0.27f));
		r7 *= -1.f; //invert
		r0 = max(r0, r7); //intersect 1
		float r8 = sdfCylinderY(p - vec3(0.198f, 0, 0.378f), vec2(0.036f, 0.27f));
		r8 *= -1.f; //invert
		r0 = max(r0, r8); //intersect 1
		float r9 = sdfCylinderY(p - vec3(0, 0.225f, 0), vec2(0.27f, 0.054f));
		float r10 = sdfCylinderY(p - vec3(0, 0.144f, 0), vec2(0.27f, 0.009f));
		r9 = min(r9, r10); //union 1
		float r11 = sdfCylinderY(p - vec3(0, 0.306f, 0), vec2(0.27f, 0.009f));
		r9 = min(r9, r11); //union 2
		float r12 = sdfCylinderY(p - vec3(0, 0.225f, 0), vec2(0.2295f, 0.09f));
		r9 = min(r9, r12); //union 3
		float r13 = sdfCylinderY(p - vec3(0, 0.288f, 0), vec2(0.162f, 0.0288f));
		r13 *= -1.f; //invert
		r9 = max(r9, r13); //intersect 1
		float r14 = sdfCylinderY(p - vec3(0, 0.162f, 0), vec2(0.162f, 0.0288f));
		r14 *= -1.f; //invert
		r9 = max(r9, r14); //intersect 1
		float r15 = sdfCylinderY(p - vec3(0, 0.225f, 0), vec2(0.1134f, 0.09f));
		r15 *= -1.f; //invert
		r9 = max(r9, r15); //intersect 1
		r0 = min(r0, r9); //union 1
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model4() final = default;

private:
};
#endif
