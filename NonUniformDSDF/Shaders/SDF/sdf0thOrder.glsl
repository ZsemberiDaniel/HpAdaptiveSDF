//?#version 460
//?#include "../common.glsl"

uniform sampler3D texTex;

// x: texture coordinates
float getSample(vec3 x)
{
	float a = texture(texTex, x).x;
	return a;
}
