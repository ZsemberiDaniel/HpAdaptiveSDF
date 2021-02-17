vec3 getBlendedWeight(vec3 r);
//!#include "weightBlend.glsl"

// x: sampling position (3D texture coordinate) <- [0,1]^3
// dimf: resolution of the 3D texture = N
// icoord: clamped index of the cell containing x. (coordinates of the min corner) <- [0, N-2]
// sp: clamped sampling position in cell coordinates <- [0, N-1]
// r: mixing coefficient <- [0,1]
void sample3dHelper(vec3 x, vec3 dimf, out ivec3 icoord, out vec3 sp, out vec3 r)
{
	sp = clamp(dimf*x, vec3(0.5), dimf-0.5) - 0.5;	// [0, N-1]
	vec3 c = min(floor(sp), dimf - 2); // [0, N-2]
	r = sp - c; // [0, 1] mixing coef
//	r = getBlendedWeight(r);
	icoord = ivec3(c);
}

ivec3 sample3dHelper_nearest(vec3 x, vec3 dimf)
{
	return ivec3(clamp(round(dimf*x - .5), vec3(0), dimf - 1));
}
