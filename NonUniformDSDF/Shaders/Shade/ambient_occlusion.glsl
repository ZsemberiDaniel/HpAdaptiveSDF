//?#version 450

const int gAOMaxIters = 5;
const float gAOStepSize = 0.4f;
const float gAOStrength = 0.7f;

//?#include "../common.glsl"

float ambient_occlusion_basic(vec3 p, vec3 n)
{
	float Occlusion = 0;
	vec3 normal = normalize(n);

	for (int i = 0; i <= gAOMaxIters; ++i)
	{
		float radius = i * gAOStepSize;
		Occlusion +=  pow(0.5, i) * ( radius - sdf(p + normal * radius) );
	}

	return (1 - clamp(gAOStrength * Occlusion, 0, 1));
}
