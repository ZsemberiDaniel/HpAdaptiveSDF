#ifndef SDFMODEL3_H
#define SDFMODEL3_H
#include <glm/glm.hpp>

#include "SDFBase.h"
#include "SDFPrimitives.h"

class SDFModel3 : public SDFBase
{
public:
	SDFModel3(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model3", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		float r0 = SDFPrimitives::sdfBox(p, glm::vec3(0.45, 0.27, 0.45));
		float r1 = SDFPrimitives::sdfCylinderY(p - glm::vec3(-0.45, 0, -0.45), glm::vec2(0.18, 0.54));
		r1 *= -1.f; //invert
		r0 = glm::max(r0, r1); //intersect 1
		float r2 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0.45, 0, -0.45), glm::vec2(0.18, 0.54));
		r2 *= -1.f; //invert
		r0 = glm::max(r0, r2); //intersect 1
		float r3 = SDFPrimitives::sdfCylinderY(p - glm::vec3(-0.45, 0, 0.45), glm::vec2(0.18, 0.54));
		r3 *= -1.f; //invert
		r0 = glm::max(r0, r3); //intersect 1
		float r4 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0.45, 0, 0.45), glm::vec2(0.18, 0.54));
		r4 *= -1.f; //invert
		r0 = glm::max(r0, r4); //intersect 1
		float r5 = SDFPrimitives::sdfCylinderY(p - glm::vec3(-0.198, 0, -0.378), glm::vec2(0.036, 0.54));
		r5 *= -1.f; //invert
		r0 = glm::max(r0, r5); //intersect 1
		float r6 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0.378, 0, -0.198), glm::vec2(0.036, 0.54));
		r6 *= -1.f; //invert
		r0 = glm::max(r0, r6); //intersect 1
		float r7 = SDFPrimitives::sdfCylinderY(p - glm::vec3(-0.378, 0, 0.198), glm::vec2(0.036, 0.54));
		r7 *= -1.f; //invert
		r0 = glm::max(r0, r7); //intersect 1
		float r8 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0.198, 0, 0.378), glm::vec2(0.036, 0.54));
		r8 *= -1.f; //invert
		r0 = glm::max(r0, r8); //intersect 1
		float r9 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.45, 0), glm::vec2(0.27, 0.108));
		float r10 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.288, 0), glm::vec2(0.27, 0.018));
		r9 = glm::min(r9, r10); //union 1
		float r11 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.612, 0), glm::vec2(0.27, 0.018));
		r9 = glm::min(r9, r11); //union 2
		float r12 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.45, 0), glm::vec2(0.2295, 0.18));
		r9 = glm::min(r9, r12); //union 3
		float r13 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.576, 0), glm::vec2(0.162, 0.0576));
		r13 *= -1.f; //invert
		r9 = glm::max(r9, r13); //intersect 1
		float r14 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.324, 0), glm::vec2(0.162, 0.0576));
		r14 *= -1.f; //invert
		r9 = glm::max(r9, r14); //intersect 1
		float r15 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.45, 0), glm::vec2(0.1134, 0.18));
		r15 *= -1.f; //invert
		r9 = glm::max(r9, r15); //intersect 1
		r0 = glm::min(r0, r9); //union 1
		return r0;
	}

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDFModel3() final = default;

private:
};
#endif