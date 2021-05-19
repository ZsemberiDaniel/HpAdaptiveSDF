#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model2 : public SDFBase
{
public:
	SDF2Model2(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model2", "SDFGPUandCPU/SDF2Model2.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.5f, 0.5f, 0.5f);
		float r0 = sdfBox(p, vec3(0.45f, 0.03f, 0.25f));
		float r1 = sdfBox(p - vec3(0.4375f, 0.15f, 0), vec3(0.0125f, 0.15f, 0.25f));
		float r2 = sdfBox(p - vec3(-0.4375f, 0.15f, 0), vec3(0.0125f, 0.15f, 0.25f));
		r1 = min(r1, r2); //union 1
		float r3 = sdfBox(p - vec3(0, 0.15f, 0.2375f), vec3(0.45f, 0.15f, 0.0125f));
		r1 = min(r1, r3); //union 2
		float r4 = sdfBox(p - vec3(0, 0.15f, -0.2375f), vec3(0.45f, 0.15f, 0.0125f));
		r1 = min(r1, r4); //union 3
		r0 = min(r0, r1); //union 1
		float r5 = sdfBox(p - vec3(-0.144f, 0.15f, 0.2125f), vec3(0.036f, 0.15f, 0.0375f));
		float r6 = sdfBox(p - vec3(-0.144f, 0.15f, -0.2125f), vec3(0.036f, 0.15f, 0.0375f));
		r5 = min(r5, r6); //union 1
		float r7 = sdfBox(p - vec3(0.144f, 0.15f, 0.2125f), vec3(0.036f, 0.15f, 0.0375f));
		r5 = min(r5, r7); //union 1
		float r8 = sdfBox(p - vec3(0.144f, 0.15f, -0.2125f), vec3(0.036f, 0.15f, 0.0375f));
		r5 = min(r5, r8); //union 2
		r0 = min(r0, r5); //union 2
		float r9 = sdfBox(p - vec3(0.3825f, 0.15f, -0.08f), vec3(0.0675f, 0.15f, 0.02f));
		float r10 = sdfBox(p - vec3(-0.3825f, 0.15f, -0.08f), vec3(0.0675f, 0.15f, 0.02f));
		r9 = min(r9, r10); //union 1
		float r11 = sdfBox(p - vec3(0.3825f, 0.15f, 0), vec3(0.0675f, 0.15f, 0.02f));
		r9 = min(r9, r11); //union 1
		float r12 = sdfBox(p - vec3(-0.3825f, 0.15f, 0), vec3(0.0675f, 0.15f, 0.02f));
		r9 = min(r9, r12); //union 2
		float r13 = sdfBox(p - vec3(0.3825f, 0.15f, 0.08f), vec3(0.0675f, 0.15f, 0.02f));
		r9 = min(r9, r13); //union 1
		float r14 = sdfBox(p - vec3(-0.3825f, 0.15f, 0.08f), vec3(0.0675f, 0.15f, 0.02f));
		r9 = min(r9, r14); //union 2
		r0 = min(r0, r9); //union 3
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model2() final = default;

private:
};
#endif
