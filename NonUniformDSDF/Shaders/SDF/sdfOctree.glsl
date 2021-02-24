//?#version 450

//?#include "../common.glsl"
//?#include "../Octree/octree.glsl"
//?#include "../Math/common.glsl"


float getSample(vec3 p)
{
	//return length(p) - 1.0f;

	Leaf leaf = searchForLeaf(p);

	Polynom poly;
	poly.degree = 1;
	poly.coeffCount = getCoeffCount(poly.degree);
	poly.coeffs[0] = 0.0f;
	poly.coeffs[1] = 0.0f;
	poly.coeffs[2] = 0.5f;
	poly.coeffs[3] = 0.0f;

	return evalPolynom(poly, p * 2.0f - 1.0f);
}
