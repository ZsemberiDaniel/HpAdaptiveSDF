//?#version 450

//?#include "../common.glsl"
//?#include "../Octree/octree.glsl"
//?#include "../Math/common.glsl"

float getSample(vec3 p)
{
	Leaf leaf = searchForLeaf(p);

	return evalPolynom(leaf.poly.coeffs, leaf.poly.degree, p);
}
