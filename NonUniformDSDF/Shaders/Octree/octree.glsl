//?#version 450

//?#include "../common.glsl"
//?#include "../Math/common.glsl"

readonly layout(std430, binding = 0) buffer branches
{
	uint br_data[];
	// level, pointer1, pointer2, ..., pointer8
	// when these pointers are outside of the length of branches, it refers to a leaf
	// last branch should be the one where the search starts (upper-most branch)
};
// size in uints of a branch in SSBO
const int BR_SIZE = 1 + 8;
uniform int branchCount = -1;

uniform vec3 octreeMinCoord = vec3(0);
uniform float octreeSize = 1;

readonly layout(std430, binding = 1) buffer leaves
{
	uint l_leaves[]; // level in octree; degree; (coeffs1, coeffs2, ...); ...;
};

//uniform uint br_data[];
//uniform uint l_leaves[];

struct Branch {
	uint level;
	uint[8] pointers;
};

struct Leaf {
	uint level;
	Polynom	poly;
};

Branch getBranch(in uint branchId)
{
	Branch br;
	uint branchIdInSSBO = branchId * BR_SIZE;
	br.level = br_data[branchIdInSSBO];
	for (int i = 0; i < 8; i++)
		br.pointers[i] = br_data[branchIdInSSBO + 1 + i];

	return br;
}

Leaf getLeaf(in uint leafIndexInArray)
{
	Leaf leaf; 
	Polynom poly;
	leaf.level =  l_leaves[leafIndexInArray + 0];
	poly.degree = l_leaves[leafIndexInArray + 1];
	poly.coeffCount = getCoeffCount(poly.degree);

	for (int i = 0; i < poly.coeffCount; i += 4)
	{
		vec4 coeffs = unpackSnorm4x8(l_leaves[leafIndexInArray + 2 + i / 4]);
		poly.coeffs[4 * i + 0] = coeffs.x;
		poly.coeffs[4 * i + 1] = coeffs.y;
		poly.coeffs[4 * i + 2] = coeffs.z;
		poly.coeffs[4 * i + 3] = coeffs.w;
	}

	leaf.poly = poly;

	return leaf;
}

Leaf searchForLeaf(vec3 p, out vec3 boxMin, out vec3 boxMax)
{
	vec3 localP = p;
	uint currentBranchId = branchCount - 1;

	boxMin = vec3(0);
	boxMax = vec3(1);

	while (currentBranchId < branchCount)
	{
		Branch br = getBranch(currentBranchId);

		vec3 boxSize = boxMax - boxMin;
		// int pointerId = int(localP.z >= 0.5f) * 4 + int(localP.y >= 0.5f) * 2 + int(localP.x >= 0.5f);
		int pointerId = 0;
		if (localP.z >= 0.5f) 
		{
			localP.z -= 0.5f;
			pointerId += 4;

			boxMin.z += boxSize.z / 2.0f;
		}
		else
		{
			boxMax.z -= boxSize.z / 2.0f;
		}

		if (localP.y >= 0.5f) 
		{
			localP.y -= 0.5f;
			pointerId += 2;

			boxMin.y += boxSize.y / 2.0f;
		}
		else
		{
			boxMax.y -= boxSize.y / 2.0f;
		}

		if (localP.x >= 0.5f) 
		{
			localP.x -= 0.5f;
			pointerId += 1;

			boxMin.x += boxSize.x / 2.0f;
		}
		else
		{
			boxMax.x -= boxSize.x / 2.0f;
		}


		localP *= 2.0f;
		currentBranchId = br.pointers[pointerId];
	}

	return getLeaf(currentBranchId - branchCount);
}
