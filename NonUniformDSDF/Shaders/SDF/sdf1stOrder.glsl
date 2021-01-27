//?#version 460
//?#include "../common.glsl"

//?#include "sample3d.glsl"

uniform sampler3D texTex;
vec3 getTexSize() { return textureSize(texTex, 0); }

// x: texture coordinates, pp: evaluation point, v1: polynomial sample
void sampling_helper(vec3 x, out vec3 pp, out vec4 v1)
{
	vec3 dimf = getTexSize();
	vec3 sp, r; ivec3 icoord;
	sample3dHelper(x, dimf, icoord, sp, r);

	v1 = texture(texTex, x).xyzw;

#if defined NORMALIZED_MONOMIAL_BASIS || defined LEGENDRE_BASIS
	pp = 2*x - 1;
#else
	pp = sdfTexCorner + (sp+.5)/dimf*sdfTexSize;
#endif
}

// x: texture coordinates
float getSample(vec3 x)
{
	vec3 pp;
	vec4 v1;
	sampling_helper(x, pp, v1);
	
	return dot(v1, vec4(1, pp));
}
