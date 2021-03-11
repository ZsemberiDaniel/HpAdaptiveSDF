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

uniform float coeffCompressAmount;
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

	for (int i = 0; i < poly.coeffCount; i += 2)
	{
//		vec4 coeffs = unpackSnorm4x8(l_leaves[leafIndexInArray + 2 + i / 4]);
//		poly.coeffs[i + 0] = coeffs.x;
//		poly.coeffs[i + 1] = coeffs.y;
//		poly.coeffs[i + 2] = coeffs.z;
//		poly.coeffs[i + 3] = coeffs.w;

		vec2 coeffs = unpackSnorm2x16(l_leaves[leafIndexInArray + 2 + i / 2]);
		poly.coeffs[i + 0] = coeffs.x * coeffCompressAmount;
		poly.coeffs[i + 1] = coeffs.y * coeffCompressAmount;
	}

	leaf.poly = poly;

	return leaf;
}

Leaf searchForLeaf(vec3 p, out vec3 boxMin, out vec3 boxMax)
{
	vec3 localP = p;
	// last element is the root
	uint currentBranchId = branchCount - 1;

	boxMin = vec3(0);
	boxMax = vec3(1);

	while (currentBranchId < branchCount)
	{
		Branch br = getBranch(currentBranchId);

		vec3 halfBoxSize = (boxMax - boxMin) / 2.0f;
		// int pointerId = int(localP.z >= 0.5f) * 4 + int(localP.y >= 0.5f) * 2 + int(localP.x >= 0.5f);
		int pointerId = 0;
		if (localP.z > 0.5f) 
		{
			localP.z -= 0.5f;
			pointerId += 4;

			boxMin.z += halfBoxSize.z;
		}
		else
		{
			boxMax.z -= halfBoxSize.z;
		}

		if (localP.y > 0.5f) 
		{
			localP.y -= 0.5f;
			pointerId += 2;

			boxMin.y += halfBoxSize.y;
		}
		else
		{
			boxMax.y -= halfBoxSize.y;
		}

		if (localP.x > 0.5f) 
		{
			localP.x -= 0.5f;
			pointerId += 1;

			boxMin.x += halfBoxSize.x;
		}
		else
		{
			boxMax.x -= halfBoxSize.x;
		}


		localP *= 2.0f;
		currentBranchId = br.pointers[pointerId];
	}

	return getLeaf(currentBranchId - branchCount);
}
