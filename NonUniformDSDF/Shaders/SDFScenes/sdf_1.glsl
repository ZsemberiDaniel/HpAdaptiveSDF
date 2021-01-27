//?#include "../Math/primitives.glsl"

// Start of generated GLSL code
float SceneSDF(vec3 p)
{
	float r0 = cylinderX(p - vec3(0,0.7,0), vec2(0.25,0.5));
	r0 -= 0.01;//offset
	float r1 = cylinderZ(p - vec3(0,0.7,0), vec2(0.25,0.5));
	r1 -= 0.01;//offset
	r0 = min(r0,r1); //union 1
	float r2 = sphere(p - vec3(0,0.7,0), 0.45);
	r0 = min(r0,r2); //union 2
	float r3 = cylinderY(p - vec3(0,0.6,0), vec2(0.25,0.6));
	r3 -= 0.01;//offset
	r0 = min(r0,r3); //union 3
	float r4 = box(p, vec3(0.4,0.1,0.4));
	r4 -= 0.01;//offset
	r0 = min(r0,r4); //union 4
return r0;
}

// End of generated GLSL code

float funDist(vec3 p)
{
	return SceneSDF(p);
}