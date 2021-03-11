#ifndef SDFPRIMITIVES_H
#define SDFPRIMITIVES_H

#include <glm/glm.hpp>

class SDFPrimitives
{
public:
	static float sdfSphere(glm::vec3 p, glm::vec3 c, float r) {
		return glm::length(p - c) - r;
	}

	static float sdfSphere(glm::vec3 p, float r) {
		return glm::length(p) - r;
	}

	static float sdfBox(glm::vec3 p, glm::vec3 b)
	{
		glm::vec3 q = abs(p) - b;
		return glm::length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
	}

	static float sdfTorus(glm::vec3 p, float R, float r)
	{
		glm::vec2 q = glm::vec2(glm::length(glm::vec2(p.x, p.z)) - R, p.y);
		return glm::length(q) - r;
	}

	static float sdfPyramid(glm::vec3 p, float h)
	{
		float m2 = h * h + 0.25;

		p.x = abs(p.x);
		p.z = abs(p.z);
		if (p.z > p.x)
		{
			float temp = p.x;
			p.x = p.z;
			p.z = temp;
		}
		p.x -= 0.5f;
		p.z -= 0.5f;

		glm::vec3 q = glm::vec3(p.z, h * p.y - 0.5f * p.x, h * p.x + 0.5f * p.y);

		float s = glm::max(-q.x, 0.0f);
		float t = glm::clamp((q.y - 0.5f * p.z) / (m2 + 0.25f), 0.0f, 1.0f);

		float a = m2 * (q.x + s) * (q.x + s) + q.y * q.y;
		float b = m2 * (q.x + 0.5 * t) * (q.x + 0.5 * t) + (q.y - m2 * t) * (q.y - m2 * t);

		float d2 = glm::min(q.y, -q.x * m2 - q.y * 0.5f) > 0.0f ? 0.0f : glm::min(a, b);

		return sqrt((d2 + q.z * q.z) / m2) * glm::sign(glm::max(q.z, -p.y));
	}

	static float sdfPlane(glm::vec3 p, glm::vec3 n)
	{
		return dot(p, normalize(n));
	}

	// Axis aligned planes
	static float sdfPlaneYZ(glm::vec3 p) { return p.x; }
	static float sdfPlaneXZ(glm::vec3 p) { return p.y; }
	static float sdfPlaneXY(glm::vec3 p) { return p.y; }

	// Infinite cylinders
	static float sdfCylinderZ(glm::vec3 p, float r) { return glm::length(glm::vec2(p.x, p.y)) - r; }
	static float sdfCylinderX(glm::vec3 p, float r) { return sdfCylinderZ(glm::vec3(p.z, p.y, p.x), r); }
	static float sdfCylinderY(glm::vec3 p, float r) { return sdfCylinderZ(glm::vec3(p.x, p.z, p.y), r); }

	// Finite cylinders
	static float sdfCylinderZ(glm::vec3 p, glm::vec2 h)
	{
		glm::vec2 d = abs(glm::vec2(glm::length(glm::vec2(p.x, p.y)), abs(p.z))) - h;
		return glm::min(glm::max(d.x, d.y), 0.0f) + glm::length(glm::max(d, 0.0f));
	}
	static float sdfCylinderX(glm::vec3 p, glm::vec2 h) { return sdfCylinderZ(glm::vec3(p.z, p.y, p.x), h); }
	static float sdfCylinderY(glm::vec3 p, glm::vec2 h) { return sdfCylinderZ(glm::vec3(p.x, p.z, p.y), h); }

	static float sdfUnion(float d1, float d2)
	{
		return glm::min(d1, d2);
	}

	static float sdfSubtraction(float d1, float d2)
	{
		return glm::max(-d1, d2);
	}

	static float sdfIntersect(float d1, float d2)
	{
		return glm::max(d1, d2);
	}
};
#endif