//?#version 450

//?#include "../common.glsl"
//?#include "../Octree/octree.glsl"
//?#include "../Math/common.glsl"

vec3 debug1(vec3 p)
{
	// return p;
	vec3 leafBoxMin, leafBoxMax;
	Leaf leaf = searchForLeaf(p, leafBoxMin, leafBoxMax);
	vec3 shiftedP = (p - leafBoxMin) / (leafBoxMax - leafBoxMin);
	// return leafBoxMin;
	// return shiftedP;
	return vec3(evalPolynom(leaf.poly, shiftedP * 2.0f - 1.0f) / octreeSize);
	// return vec3(leaf.level / 10.0f);
}

float getSample(vec3 p)
{
//	const float R = 0.8f;
//	const float r = 0.4f;
//	vec2 q = vec2(length(vec2(p.x, p.z)) - R, p.y);
//	return length(q) - r;

	// return length(p) - 1.0f;

	vec3 leafBoxMin, leafBoxMax;
	Leaf leaf = searchForLeaf(p, leafBoxMin, leafBoxMax);
	vec3 shiftedP = (p - leafBoxMin) / (leafBoxMax - leafBoxMin);
	return evalPolynom(leaf.poly, p * 2.0f - 1.0f) / octreeSize;

//	Polynom poly;
//	poly.degree = 1;
//	poly.coeffCount = getCoeffCount(poly.degree);
//	poly.coeffs[0] = 0.0f;
//	poly.coeffs[1] = 0.0f;
//	poly.coeffs[2] = 0.5f;
//	poly.coeffs[3] = 0.0f;
//
//	return evalPolynom(poly, p * 2.0f - 1.0f);
}
