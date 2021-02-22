//?#version 450

//?#include "../common.glsl"
//?#include "../Math/common.glsl"

// When inputting these need to be scaled so the root has 1x1x1 size
// TODO: may have problems with padding
layout(std430, binding = 0) buffer branches
{
	uint br_data[];
	// level, pointer1, pointer2, ..., pointer8
	// when these pointers are outside of the length of branches, it refers to a leaf
};
// size in uints of a branch in SSBO
const int BR_SIZE = 5;
uniform int branchCount = -1;

layout(std430, binding = 1) buffer leaves
{
	uint l_leaves[]; // level in octree; degree; (coeffs1, coeffs2, ...); ...;
};

struct Branch {
	uint level;
	uint[8] pointers;
};

const int MAX_COEFF_SIZE = 286; // deg 10
struct Polynom {
	uint degree;
	uint coeffCount;
	float[MAX_COEFF_SIZE] coeffs;
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
	Leaf leaf; Polynom poly;
	leaf.level = l_leaves[leafIndexInArray];
	poly.degree = l_leaves[leafIndexInArray + 1];
	poly.coeffCount = getCoeffCount(poly.degree);

	for (int i = 0; i < poly.coeffCount; i += 4)
	{
		vec4 coeffs = unpackSnorm4x8(l_leaves[leafIndexInArray + 2 + i / 4]);
		poly.coeffs[i + 0] = coeffs.x;
		poly.coeffs[i + 1] = coeffs.y;
		poly.coeffs[i + 2] = coeffs.z;
		poly.coeffs[i + 3] = coeffs.w;
	}

	return leaf;
}

Leaf searchForLeaf(vec3 p)
{
	vec3 localP = p;
	uint currentBranchId = 0;
	while (currentBranchId < branchCount)
	{
		Branch br = getBranch(currentBranchId);

		// int pointerId = int(localP.z >= 0.5f) * 4 + int(localP.y >= 0.5f) * 2 + int(localP.x >= 0.5f);
		int pointerId = 0;
		if (localP.z >= 0.5f) 
		{
			localP.z -= 0.5f;
			pointerId += 4;
		}
		if (localP.y >= 0.5f) 
		{
			localP.y -= 0.5f;
			pointerId += 2;
		}
		if (localP.x >= 0.5f) 
		{
			localP.x -= 0.5f;
			pointerId += 1;
		}

		localP *= 2.0f;
		currentBranchId = br.pointers[pointerId];
	}

	return getLeaf(currentBranchId - branchCount);
}
