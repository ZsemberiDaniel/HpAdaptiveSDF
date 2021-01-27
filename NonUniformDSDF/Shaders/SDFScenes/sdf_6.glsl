//?#include "../Math/primitives.glsl"

// Start of generated GLSL code
float SceneSDF(vec3 p)
{
	float r0 = box(p, vec3(0.5,0.03,0.25));
	float r1 = box(p - vec3(0.4875,0.15,0), vec3(0.0125,0.15,0.25));
	float r2 = box(p - vec3(-0.4875,0.15,0), vec3(0.0125,0.15,0.25));
	r1 = min(r1,r2); //union 1
	float r3 = box(p - vec3(0,0.15,0.2375), vec3(0.5,0.15,0.0125));
	r1 = min(r1,r3); //union 2
	float r4 = box(p - vec3(0,0.15,-0.2375), vec3(0.5,0.15,0.0125));
	r1 = min(r1,r4); //union 3
	r0 = min(r0,r1); //union 1
	float r5 = box(p - vec3(-0.181818,0.15,0.2125), vec3(0.0181818,0.15,0.0375));
	float r6 = box(p - vec3(-0.181818,0.15,-0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r6); //union 1
	float r7 = box(p - vec3(-0.109091,0.15,0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r7); //union 1
	float r8 = box(p - vec3(-0.109091,0.15,-0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r8); //union 2
	float r9 = box(p - vec3(0.109091,0.15,0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r9); //union 1
	float r10 = box(p - vec3(0.109091,0.15,-0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r10); //union 2
	float r11 = box(p - vec3(0.181818,0.15,0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r11); //union 1
	float r12 = box(p - vec3(0.181818,0.15,-0.2125), vec3(0.0181818,0.15,0.0375));
	r5 = min(r5,r12); //union 2
	r0 = min(r0,r5); //union 2
	float r13 = box(p - vec3(0.425,0.15,-0.08), vec3(0.075,0.15,0.02));
	float r14 = box(p - vec3(-0.425,0.15,-0.08), vec3(0.075,0.15,0.02));
	r13 = min(r13,r14); //union 1
	float r15 = box(p - vec3(0.425,0.15,0), vec3(0.075,0.15,0.02));
	r13 = min(r13,r15); //union 1
	float r16 = box(p - vec3(-0.425,0.15,0), vec3(0.075,0.15,0.02));
	r13 = min(r13,r16); //union 2
	float r17 = box(p - vec3(0.425,0.15,0.08), vec3(0.075,0.15,0.02));
	r13 = min(r13,r17); //union 1
	float r18 = box(p - vec3(-0.425,0.15,0.08), vec3(0.075,0.15,0.02));
	r13 = min(r13,r18); //union 2
	r0 = min(r0,r13); //union 3
return r0;
}

// End of generated GLSL code

float funDist(vec3 p)
{
	return SceneSDF(p);
}