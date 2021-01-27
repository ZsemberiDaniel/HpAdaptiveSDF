//!#include "weightBlend.glsl"

void sample3dHelper(vec3 x, vec3 dimf, out ivec3 icoord, out vec3 sp, out vec3 r)
{
	sp = clamp(dimf*x, vec3(0.5), dimf-0.5) - 0.5;	// [0, N-1]
	vec3 c = min(floor(sp), dimf - 2); // [0, N-2]
	r = sp - c; // [0, 1] mixing coef
	r = getBlendedWeight(r);
	icoord = ivec3(c);
}

ivec3 sample3dHelper_nearest(vec3 x, vec3 dimf)
{
	return ivec3(clamp(round(dimf*x - .5), vec3(0), dimf - 1));
}
