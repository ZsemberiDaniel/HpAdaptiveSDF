//?#include "../Math/primitives.glsl"

// Start of generated GLSL code
float SceneSDF(vec3 p)
{
	float r0 = box(p, vec3(3.5,3.5,3.5));
	float r1 = sphere(p, 5);
	r0 = max(r0,r1); //intersect 1
	r0 -= 0.5;//offset
return r0;
}

// End of generated GLSL code

float funDist(vec3 p)
{
	return SceneSDF(p);
}