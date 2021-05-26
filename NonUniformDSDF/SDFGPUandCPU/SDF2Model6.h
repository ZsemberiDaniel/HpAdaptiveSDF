#pragma once

#ifndef GPU_SIDE
#include <glm/glm.hpp>

#include "../SDF/SDFBase.h"
#include "../SDF/SDFPrimitives.h"


class SDF2Model6 : public SDFBase
{
public:
	SDF2Model6(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model6", "SDFGPUandCPU/SDF2Model6.h", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		using namespace SDFPrimitives;
		using namespace glm;
#else
	float SceneSDF(vec3 p) {
#endif
		p -= vec3(0.2f, 0.5f, 0.2f); 
		float r0 = sdfBox(p - vec3(0.612f, -0.135f, 0.612f), vec3(0.09f, 0.015f, 0.09f));
		float r1 = sdfBox(p - vec3(0.612f, 0, 0.612f), vec3(0.09f, 0.15f, 0.09f));
		r1 -= 0.009f;//offset
		float r2 = sdfBox(p - vec3(0.612f, 0, 0.612f), vec3(0.081f, 0.3f, 0.081f));
		r2 *= -1.f; //invert
		r1 = max(r1, r2); //intersect 1
		r0 = min(r0, r1); //union 1
		float r3 = sdfCylinderX(p - vec3(0.612f, -0.075f, 0.612f), vec2(0.018f, 0.216f));
		r3 *= -1.f; //invert
		r0 = max(r0, r3); //intersect 1
		float r4 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0.612f), vec2(0.018f, 0.216f));
		r4 *= -1.f; //invert
		r0 = max(r0, r4); //intersect 1
		float r5 = sdfBox(p - vec3(0.612f, -0.135f, 0.306f), vec3(0.09f, 0.015f, 0.09f));
		float r6 = sdfBox(p - vec3(0.612f, 0, 0.306f), vec3(0.09f, 0.15f, 0.09f));
		r6 -= 0.009f;//offset
		float r7 = sdfBox(p - vec3(0.612f, 0, 0.306f), vec3(0.081f, 0.3f, 0.081f));
		r7 *= -1.f; //invert
		r6 = max(r6, r7); //intersect 1
		r5 = min(r5, r6); //union 1
		float r8 = sdfCylinderX(p - vec3(0.612f, -0.075f, 0.306f), vec2(0.018f, 0.216f));
		r8 *= -1.f; //invert
		r5 = max(r5, r8); //intersect 1
		float r9 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0.306f), vec2(0.018f, 0.216f));
		r9 *= -1.f; //invert
		r5 = max(r5, r9); //intersect 1
		float r10 = sdfBox(p - vec3(0.612f, -0.135f, 0), vec3(0.09f, 0.015f, 0.09f));
		float r11 = sdfBox(p - vec3(0.612f, 0, 0), vec3(0.09f, 0.15f, 0.09f));
		r11 -= 0.009f;//offset
		float r12 = sdfBox(p - vec3(0.612f, 0, 0), vec3(0.081f, 0.3f, 0.081f));
		r12 *= -1.f; //invert
		r11 = max(r11, r12); //intersect 1
		r10 = min(r10, r11); //union 1
		float r13 = sdfCylinderX(p - vec3(0.612f, -0.075f, 0), vec2(0.018f, 0.216f));
		r13 *= -1.f; //invert
		r10 = max(r10, r13); //intersect 1
		float r14 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0), vec2(0.018f, 0.216f));
		r14 *= -1.f; //invert
		r10 = max(r10, r14); //intersect 1
		float r15 = sdfBox(p - vec3(0.306f, -0.135f, 0.612f), vec3(0.09f, 0.015f, 0.09f));
		float r16 = sdfBox(p - vec3(0.306f, 0, 0.612f), vec3(0.09f, 0.15f, 0.09f));
		r16 -= 0.009f;//offset
		float r17 = sdfBox(p - vec3(0.306f, 0, 0.612f), vec3(0.081f, 0.3f, 0.081f));
		r17 *= -1.f; //invert
		r16 = max(r16, r17); //intersect 1
		r15 = min(r15, r16); //union 1
		float r18 = sdfCylinderX(p - vec3(0.306f, -0.075f, 0.612f), vec2(0.018f, 0.216f));
		r18 *= -1.f; //invert
		r15 = max(r15, r18); //intersect 1
		float r19 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0.612f), vec2(0.018f, 0.216f));
		r19 *= -1.f; //invert
		r15 = max(r15, r19); //intersect 1
		float r20 = sdfBox(p - vec3(0.306f, -0.135f, 0.306f), vec3(0.09f, 0.015f, 0.09f));
		float r21 = sdfBox(p - vec3(0.306f, 0, 0.306f), vec3(0.09f, 0.15f, 0.09f));
		r21 -= 0.009f;//offset
		float r22 = sdfBox(p - vec3(0.306f, 0, 0.306f), vec3(0.081f, 0.3f, 0.081f));
		r22 *= -1.f; //invert
		r21 = max(r21, r22); //intersect 1
		r20 = min(r20, r21); //union 1
		float r23 = sdfCylinderX(p - vec3(0.306f, -0.075f, 0.306f), vec2(0.018f, 0.216f));
		r23 *= -1.f; //invert
		r20 = max(r20, r23); //intersect 1
		float r24 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0.306f), vec2(0.018f, 0.216f));
		r24 *= -1.f; //invert
		r20 = max(r20, r24); //intersect 1
		float r25 = sdfBox(p - vec3(0.306f, -0.135f, 0), vec3(0.09f, 0.015f, 0.09f));
		float r26 = sdfBox(p - vec3(0.306f, 0, 0), vec3(0.09f, 0.15f, 0.09f));
		r26 -= 0.009f;//offset
		float r27 = sdfBox(p - vec3(0.306f, 0, 0), vec3(0.081f, 0.3f, 0.081f));
		r27 *= -1.f; //invert
		r26 = max(r26, r27); //intersect 1
		r25 = min(r25, r26); //union 1
		float r28 = sdfCylinderX(p - vec3(0.306f, -0.075f, 0), vec2(0.018f, 0.216f));
		r28 *= -1.f; //invert
		r25 = max(r25, r28); //intersect 1
		float r29 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0), vec2(0.018f, 0.216f));
		r29 *= -1.f; //invert
		r25 = max(r25, r29); //intersect 1
		float r30 = sdfBox(p - vec3(0, -0.135f, 0.612f), vec3(0.09f, 0.015f, 0.09f));
		float r31 = sdfBox(p - vec3(0, 0, 0.612f), vec3(0.09f, 0.15f, 0.09f));
		r31 -= 0.009f;//offset
		float r32 = sdfBox(p - vec3(0, 0, 0.612f), vec3(0.081f, 0.3f, 0.081f));
		r32 *= -1.f; //invert
		r31 = max(r31, r32); //intersect 1
		r30 = min(r30, r31); //union 1
		float r33 = sdfCylinderX(p - vec3(0, -0.075f, 0.612f), vec2(0.018f, 0.216f));
		r33 *= -1.f; //invert
		r30 = max(r30, r33); //intersect 1
		float r34 = sdfCylinderZ(p - vec3(0, -0.075f, 0.612f), vec2(0.018f, 0.216f));
		r34 *= -1.f; //invert
		r30 = max(r30, r34); //intersect 1
		float r35 = sdfBox(p - vec3(0, -0.135f, 0.306f), vec3(0.09f, 0.015f, 0.09f));
		float r36 = sdfBox(p - vec3(0, 0, 0.306f), vec3(0.09f, 0.15f, 0.09f));
		r36 -= 0.009f;//offset
		float r37 = sdfBox(p - vec3(0, 0, 0.306f), vec3(0.081f, 0.3f, 0.081f));
		r37 *= -1.f; //invert
		r36 = max(r36, r37); //intersect 1
		r35 = min(r35, r36); //union 1
		float r38 = sdfCylinderX(p - vec3(0, -0.075f, 0.306f), vec2(0.018f, 0.216f));
		r38 *= -1.f; //invert
		r35 = max(r35, r38); //intersect 1
		float r39 = sdfCylinderZ(p - vec3(0, -0.075f, 0.306f), vec2(0.018f, 0.216f));
		r39 *= -1.f; //invert
		r35 = max(r35, r39); //intersect 1
		float r40 = sdfBox(p - vec3(0, -0.135f, 0), vec3(0.09f, 0.015f, 0.09f));
		float r41 = sdfBox(p, vec3(0.09f, 0.15f, 0.09f));
		r41 -= 0.009f;//offset
		float r42 = sdfBox(p, vec3(0.081f, 0.3f, 0.081f));
		r42 *= -1.f; //invert
		r41 = max(r41, r42); //intersect 1
		r40 = min(r40, r41); //union 1
		float r43 = sdfCylinderX(p - vec3(0, -0.075f, 0), vec2(0.018f, 0.216f));
		r43 *= -1.f; //invert
		r40 = max(r40, r43); //intersect 1
		float r44 = sdfCylinderZ(p - vec3(0, -0.075f, 0), vec2(0.018f, 0.216f));
		r44 *= -1.f; //invert
		r40 = max(r40, r44); //intersect 1
		float r45 = sdfCylinderX(p - vec3(0.153f, -0.075f, 0), vec2(0.018f, 0.063f));
		float r46 = sdfCylinderX(p - vec3(0.153f, -0.075f, 0), vec2(0.0162f, 0.126f));
		r46 *= -1.f; //invert
		r45 = max(r45, r46); //intersect 1
		r40 = min(r40, r45); //union 1
		float r47 = sdfCylinderZ(p - vec3(0, -0.075f, 0.153f), vec2(0.018f, 0.063f));
		float r48 = sdfCylinderZ(p - vec3(0, -0.075f, 0.153f), vec2(0.0162f, 0.126f));
		r48 *= -1.f; //invert
		r47 = max(r47, r48); //intersect 1
		r40 = min(r40, r47); //union 1
		r35 = min(r35, r40); //union 1
		float r49 = sdfCylinderX(p - vec3(0.153f, -0.075f, 0.306f), vec2(0.018f, 0.063f));
		float r50 = sdfCylinderX(p - vec3(0.153f, -0.075f, 0.306f), vec2(0.0162f, 0.126f));
		r50 *= -1.f; //invert
		r49 = max(r49, r50); //intersect 1
		r35 = min(r35, r49); //union 1
		float r51 = sdfCylinderZ(p - vec3(0, -0.075f, 0.459f), vec2(0.018f, 0.063f));
		float r52 = sdfCylinderZ(p - vec3(0, -0.075f, 0.459f), vec2(0.0162f, 0.126f));
		r52 *= -1.f; //invert
		r51 = max(r51, r52); //intersect 1
		r35 = min(r35, r51); //union 1
		r30 = min(r30, r35); //union 1
		float r53 = sdfCylinderX(p - vec3(0.153f, -0.075f, 0.612f), vec2(0.018f, 0.063f));
		float r54 = sdfCylinderX(p - vec3(0.153f, -0.075f, 0.612f), vec2(0.0162f, 0.126f));
		r54 *= -1.f; //invert
		r53 = max(r53, r54); //intersect 1
		r30 = min(r30, r53); //union 1
		r25 = min(r25, r30); //union 1
		float r55 = sdfCylinderX(p - vec3(0.459f, -0.075f, 0), vec2(0.018f, 0.063f));
		float r56 = sdfCylinderX(p - vec3(0.459f, -0.075f, 0), vec2(0.0162f, 0.126f));
		r56 *= -1.f; //invert
		r55 = max(r55, r56); //intersect 1
		r25 = min(r25, r55); //union 1
		float r57 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0.153f), vec2(0.018f, 0.063f));
		float r58 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0.153f), vec2(0.0162f, 0.126f));
		r58 *= -1.f; //invert
		r57 = max(r57, r58); //intersect 1
		r25 = min(r25, r57); //union 1
		r20 = min(r20, r25); //union 1
		float r59 = sdfCylinderX(p - vec3(0.459f, -0.075f, 0.306f), vec2(0.018f, 0.063f));
		float r60 = sdfCylinderX(p - vec3(0.459f, -0.075f, 0.306f), vec2(0.0162f, 0.126f));
		r60 *= -1.f; //invert
		r59 = max(r59, r60); //intersect 1
		r20 = min(r20, r59); //union 1
		float r61 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0.459f), vec2(0.018f, 0.063f));
		float r62 = sdfCylinderZ(p - vec3(0.306f, -0.075f, 0.459f), vec2(0.0162f, 0.126f));
		r62 *= -1.f; //invert
		r61 = max(r61, r62); //intersect 1
		r20 = min(r20, r61); //union 1
		r15 = min(r15, r20); //union 1
		float r63 = sdfCylinderX(p - vec3(0.459f, -0.075f, 0.612f), vec2(0.018f, 0.063f));
		float r64 = sdfCylinderX(p - vec3(0.459f, -0.075f, 0.612f), vec2(0.0162f, 0.126f));
		r64 *= -1.f; //invert
		r63 = max(r63, r64); //intersect 1
		r15 = min(r15, r63); //union 1
		r10 = min(r10, r15); //union 1
		float r65 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0.153f), vec2(0.018f, 0.063f));
		float r66 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0.153f), vec2(0.0162f, 0.126f));
		r66 *= -1.f; //invert
		r65 = max(r65, r66); //intersect 1
		r10 = min(r10, r65); //union 1
		r5 = min(r5, r10); //union 1
		float r67 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0.459f), vec2(0.018f, 0.063f));
		float r68 = sdfCylinderZ(p - vec3(0.612f, -0.075f, 0.459f), vec2(0.0162f, 0.126f));
		r68 *= -1.f; //invert
		r67 = max(r67, r68); //intersect 1
		r5 = min(r5, r67); //union 1
		r0 = min(r0, r5); //union 1
		return r0;
	}
#ifndef GPU_SIDE

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDF2Model6() final = default;

private:
};
#endif
