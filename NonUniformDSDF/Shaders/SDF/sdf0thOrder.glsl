//?#version 450
//?#include "../common.glsl"

layout(binding=0) uniform sampler3D sdf0thOrderTex;

// x: texture coordinates
float getSample(vec3 x)
{
	float a = texture(sdf0thOrderTex, x).x;
	return a;
}
