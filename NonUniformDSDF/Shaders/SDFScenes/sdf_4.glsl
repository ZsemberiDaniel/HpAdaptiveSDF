//?#include "../Math/primitives.glsl"

// Start of generated GLSL code
float SceneSDF(vec3 p)
{
	float r0 = box(p - vec3(0,-0.45,0), vec3(0.5,0.05,0.5));
	float r1 = box(p, vec3(0.5,0.5,0.5));
	r1 -= 0.05;//offset
	float r2 = box(p, vec3(0.45,1,0.45));
	r2 *= -1.f; //invert
	r1 = max(r1,r2); //intersect 1
	r0 = min(r0,r1); //union 1
	float r3 = cylinderX(p - vec3(0,-0.25,0), vec2(0.1,1.2));
	r3 *= -1.f; //invert
	r0 = max(r0,r3); //intersect 1
	float r4 = cylinderZ(p - vec3(0,-0.25,0), vec2(0.1,1.2));
	r4 *= -1.f; //invert
	r0 = max(r0,r4); //intersect 1
return r0;
}

// End of generated GLSL code

float funDist(vec3 p)
{
	return SceneSDF(p);
}