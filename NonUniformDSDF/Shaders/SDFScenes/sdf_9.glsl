//?#include "../Math/primitives.glsl"

// Start of generated GLSL code
float SceneSDF(vec3 p)
{
	float r0 = sphere(p - vec3(2.5,0,0), 6);
	float r1 = cylinderZ(p - vec3(2.5,0,0), 5.8);
	r1 *= -1.f; //invert
	r0 = max(r0,r1); //intersect 1
	r0 -= 0.2;//offset
return r0;
}

// End of generated GLSL code

float funDist(vec3 p)
{
	return SceneSDF(p);
}