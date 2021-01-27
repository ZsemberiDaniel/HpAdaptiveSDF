//?#include "../Math/primitives.glsl"

// Start of generated GLSL code
float SceneSDF(vec3 p)
{
	float r0 = cylinderY(p, vec2(1,0.105));
	float r1 = cylinderY(p, vec2(0.3,0.21));
	r1 *= -1.f; //invert
	r0 = max(r0,r1); //intersect 1
	float r2 = cylinderY(p, vec2(0.3,0.126));
	float r3 = cylinderY(p, vec2(0.27,0.252));
	r3 *= -1.f; //invert
	r2 = max(r2,r3); //intersect 1
	r0 = min(r0,r2); //union 1
	float r4 = cylinderY(p - vec3(0.66,0.105,0), vec2(0.25,0.0735));
	float r5 = cylinderY(p - vec3(0.66,-0.105,0), vec2(0.25,0.0735));
	r4 = min(r4,r5); //union 1
	float r6 = cylinderY(p - vec3(0.66,0,0), vec2(0.175,0.21));
	r4 = min(r4,r6); //union 2
	r4 *= -1.f; //invert
	r0 = max(r0,r4); //intersect 1
	float r7 = cylinderY(p - vec3(-2.88495e-08,0.15,0.66), vec2(0.0875,0.09));
	float r8 = cylinderY(p - vec3(-2.88495e-08,0.15,0.66), vec2(0.08,0.18));
	r8 *= -1.f; //invert
	r7 = max(r7,r8); //intersect 1
	r0 = min(r0,r7); //union 1
	float r9 = cylinderY(p - vec3(-0.66,0.105,-5.7699e-08), vec2(0.25,0.0735));
	float r10 = cylinderY(p - vec3(-0.66,-0.105,-5.7699e-08), vec2(0.25,0.0735));
	r9 = min(r9,r10); //union 1
	float r11 = cylinderY(p - vec3(-0.66,0,-5.7699e-08), vec2(0.175,0.21));
	r9 = min(r9,r11); //union 2
	r9 *= -1.f; //invert
	r0 = max(r0,r9); //intersect 1
	float r12 = cylinderY(p - vec3(7.87042e-09,0.15,-0.66), vec2(0.0875,0.09));
	float r13 = cylinderY(p - vec3(7.87042e-09,0.15,-0.66), vec2(0.08,0.18));
	r13 *= -1.f; //invert
	r12 = max(r12,r13); //intersect 1
	r0 = min(r0,r12); //union 1
return r0;
}

// End of generated GLSL code

float funDist(vec3 p)
{
	return SceneSDF(p);
}