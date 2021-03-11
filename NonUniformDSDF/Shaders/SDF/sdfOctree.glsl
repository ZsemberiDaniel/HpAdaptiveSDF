//?#version 450

//?#include "../common.glsl"
//?#include "../Octree/octree.glsl"
//?#include "../Math/common.glsl"

vec3 debug1(vec3 p)
{
	return p;
	vec3 leafBoxMin, leafBoxMax;
	int a;
	Leaf leaf = searchForLeaf(p, leafBoxMin, leafBoxMax);
	vec3 shiftedP = (p - leafBoxMin) / (leafBoxMax - leafBoxMin);
	// return vec3(leaf.poly.coeffs[0], leaf.poly.coeffs[1], leaf.poly.coeffs[2]);
	// return leafBoxMax;
	// return shiftedP;
	// return vec3(shiftedP * 2.0f - 1.0f);
	// return vec3(evalPolynom(leaf.poly, shiftedP * 2.0f - 1.0f, (leafBoxMax - leafBoxMin)));
	// return vec3(leaf.level / 10.0f);
}

float getSample(vec3 p)
{
	vec3 leafBoxMin, leafBoxMax;
	Leaf leaf = searchForLeaf(p, leafBoxMin, leafBoxMax);
	vec3 shiftedP = (p - leafBoxMin) / (leafBoxMax - leafBoxMin);
	// cool:
	// return length(p - (leafBoxMin + leafBoxMax) / 2.0f) - (leafBoxMax - leafBoxMin).x / 4.0f;
	return evalPolynom(leaf.poly, shiftedP * 2.0f - 1.0f, (leafBoxMax - leafBoxMin));


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
