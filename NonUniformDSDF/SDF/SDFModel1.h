#ifndef SDFMODEL1_H
#define SDFMODEL1_H
#include <glm/glm.hpp>

#include "SDFBase.h"
#include "SDFPrimitives.h"

class SDFModel1 : public SDFBase
{
public:
	SDFModel1(glm::vec3 worldMinPos = glm::vec3(0), float worldSize = 1.0f) :
		SDFBase("Model1", worldMinPos, worldSize) {}

	float operator()(glm::vec3 p) const final
	{
		p -= glm::vec3(0.5f, 0.0f, 0.5f);
		float r0 = SDFPrimitives::sdfCylinderX(p - glm::vec3(0, 0.65, 0), glm::vec2(0.2, 0.5));
		r0 -= 0.01;//offset
		float r1 = SDFPrimitives::sdfCylinderZ(p - glm::vec3(0, 0.65, 0), glm::vec2(0.2, 0.5));
		r1 -= 0.01;//offset
		r0 = glm::min(r0, r1); //union 1
		float r2 = SDFPrimitives::sdfSphere(p - glm::vec3(0, 0.65, 0), 0.44);
		r0 = glm::min(r0, r2); //union 2
		float r3 = SDFPrimitives::sdfCylinderY(p - glm::vec3(0, 0.575, 0), glm::vec2(0.2, 0.575));
		r3 -= 0.01;//offset
		r0 = glm::min(r0, r3); //union 3
		float r4 = SDFPrimitives::sdfBox(p, glm::vec3(0.35, 0.075, 0.35));
		r4 -= 0.01;//offset
		r0 = glm::min(r0, r4); //union 4
		return r0;
	}

	void renderGUI() final
	{
		SDFBase::renderGUI();
	}

	~SDFModel1() final = default;

private:
};
#endif