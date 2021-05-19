#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model3 : public SDFBase
{
public:
	SDF2Model3(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model3", "SDFGPUandCPU/SDF2Model3.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.5f, 0.5f, 0.5f);
		float r0 = sdfCylinderY(p, vec2(0.45f, 0.105f));
		float r1 = sdfCylinderY(p, vec2(0.135f, 0.21f));
		r1 *= -1.f; //invert
		r0 = max(r0, r1); //intersect 1
		float r2 = sdfCylinderY(p, vec2(0.135f, 0.126f));
		float r3 = sdfCylinderY(p, vec2(0.1215f, 0.252f));
		r3 *= -1.f; //invert
		r2 = max(r2, r3); //intersect 1
		r0 = min(r0, r2); //union 1
		float r4 = sdfCylinderY(p - vec3(0.297f, 0.105f, 0), vec2(0.1125f, 0.0735f));
		float r5 = sdfCylinderY(p - vec3(0.297f, -0.105f, 0), vec2(0.1125f, 0.0735f));
		r4 = min(r4, r5); //union 1
		float r6 = sdfCylinderY(p - vec3(0.297f, 0, 0), vec2(0.07875f, 0.21f));
		r4 = min(r4, r6); //union 2
		r4 *= -1.f; //invert
		r0 = max(r0, r4); //intersect 1
		float r7 = sdfCylinderY(p - vec3(0.240278f, 0.15f, 0.174572f), vec2(0.039375f, 0.09f));
		float r8 = sdfCylinderY(p - vec3(0.240278f, 0.15f, 0.174572f), vec2(0.036f, 0.18f));
		r8 *= -1.f; //invert
		r7 = max(r7, r8); //intersect 1
		r0 = min(r0, r7); //union 1
		float r9 = sdfCylinderY(p - vec3(0.091778f, 0.105f, 0.282464f), vec2(0.1125f, 0.0735f));
		float r10 = sdfCylinderY(p - vec3(0.091778f, -0.105f, 0.282464f), vec2(0.1125f, 0.0735f));
		r9 = min(r9, r10); //union 1
		float r11 = sdfCylinderY(p - vec3(0.091778f, 0, 0.282464f), vec2(0.07875f, 0.21f));
		r9 = min(r9, r11); //union 2
		r9 *= -1.f; //invert
		r0 = max(r0, r9); //intersect 1
		float r12 = sdfCylinderY(p - vec3(-0.0917781f, 0.15f, 0.282464f), vec2(0.039375f, 0.09f));
		float r13 = sdfCylinderY(p - vec3(-0.0917781f, 0.15f, 0.282464f), vec2(0.036f, 0.18f));
		r13 *= -1.f; //invert
		r12 = max(r12, r13); //intersect 1
		r0 = min(r0, r12); //union 1
		float r14 = sdfCylinderY(p - vec3(-0.240278f, 0.105f, 0.174572f), vec2(0.1125f, 0.0735f));
		float r15 = sdfCylinderY(p - vec3(-0.240278f, -0.105f, 0.174572f), vec2(0.1125f, 0.0735f));
		r14 = min(r14, r15); //union 1
		float r16 = sdfCylinderY(p - vec3(-0.240278f, 0, 0.174572f), vec2(0.07875f, 0.21f));
		r14 = min(r14, r16); //union 2
		r14 *= -1.f; //invert
		r0 = max(r0, r14); //intersect 1
		float r17 = sdfCylinderY(p - vec3(-0.297f, 0.15f, 0.0f), vec2(0.039375f, 0.09f));
		float r18 = sdfCylinderY(p - vec3(-0.297f, 0.15f, 0.0f), vec2(0.036f, 0.18f));
		r18 *= -1.f; //invert
		r17 = max(r17, r18); //intersect 1
		r0 = min(r0, r17); //union 1
		float r19 = sdfCylinderY(p - vec3(-0.240278f, 0.105f, -0.174572f), vec2(0.1125f, 0.0735f));
		float r20 = sdfCylinderY(p - vec3(-0.240278f, -0.105f, -0.174572f), vec2(0.1125f, 0.0735f));
		r19 = min(r19, r20); //union 1
		float r21 = sdfCylinderY(p - vec3(-0.240278f, 0, -0.174572f), vec2(0.07875f, 0.21f));
		r19 = min(r19, r21); //union 2
		r19 *= -1.f; //invert
		r0 = max(r0, r19); //intersect 1
		float r22 = sdfCylinderY(p - vec3(-0.0917781f, 0.15f, -0.282464f), vec2(0.039375f, 0.09f));
		float r23 = sdfCylinderY(p - vec3(-0.0917781f, 0.15f, -0.282464f), vec2(0.036f, 0.18f));
		r23 *= -1.f; //invert
		r22 = max(r22, r23); //intersect 1
		r0 = min(r0, r22); //union 1
		float r24 = sdfCylinderY(p - vec3(0.0917781f, 0.105f, -0.282464f), vec2(0.1125f, 0.0735f));
		float r25 = sdfCylinderY(p - vec3(0.0917781f, -0.105f, -0.282464f), vec2(0.1125f, 0.0735f));
		r24 = min(r24, r25); //union 1
		float r26 = sdfCylinderY(p - vec3(0.0917781f, 0, -0.282464f), vec2(0.07875f, 0.21f));
		r24 = min(r24, r26); //union 2
		r24 *= -1.f; //invert
		r0 = max(r0, r24); //intersect 1
		float r27 = sdfCylinderY(p - vec3(0.240278f, 0.15f, -0.174572f), vec2(0.039375f, 0.09f));
		float r28 = sdfCylinderY(p - vec3(0.240278f, 0.15f, -0.174572f), vec2(0.036f, 0.18f));
		r28 *= -1.f; //invert
		r27 = max(r27, r28); //intersect 1
		r0 = min(r0, r27); //union 1
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model3() final = default;

private:
};
#endif
