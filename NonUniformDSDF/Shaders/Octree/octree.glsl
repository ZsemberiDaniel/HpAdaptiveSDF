//?#version 450

//?#include "../common.glsl"
//?#include "../Math/common.glsl"

#ifndef USE_LOOKUP_TABLE
	readonly restrict layout(std430, binding = 0) buffer branches
	{
		uint br_data[];
		// level, pointer1, pointer2, ..., pointer8
		// when these pointers are outside of the length of branches, it refers to a leaf
		// last branch should be the one where the search starts (upper-most branch)
	};
	// size in uints of a branch in SSBO
	const int BR_SIZE = 1 + 8;
	uniform int branchCount = -1;
#else
	// the lookup table
	uniform int lookupTableSize = 2;
	layout(binding = 2) uniform usampler3D lookupTable;
#endif

uniform float coeffCompressAmount;
readonly restrict layout(std430, binding = 1) buffer leaves
{
	uint l_leaves[]; // level in octree and pos.x; pos.y and pos.z; degree; (coeffs1, coeffs2, ...); ...;
};

struct Leaf {
	uint level;
	vec3 pos;
	Polynom	poly;
};

struct Branch {
	uint level;
	uint[8] pointers;
};

#ifndef USE_LOOKUP_TABLE
	Branch getBranch(in uint branchId)
	{
		Branch br;
		uint branchIdInSSBO = branchId * BR_SIZE;
		br.level = br_data[branchIdInSSBO];
		for (int i = 0; i < 8; i++)
			br.pointers[i] = br_data[branchIdInSSBO + 1 + i];

		return br;
	}

	uint getBranchLevel(in uint branchId)
	{
		return br_data[branchId * BR_SIZE];
	}
#endif

Leaf getLeaf(in uint leafIndexInArray)
{
	// IF THIS IS CHANGED:
	// the loading a save file part needs to be changed as well
	// namely inline static std::shared_ptr<SaveableOctree> initFromFileData(FileData& data)
	Leaf leaf; 
	Polynom poly;
	leaf.level = l_leaves[leafIndexInArray + 0];
	leaf.pos.x = uintBitsToFloat(l_leaves[leafIndexInArray + 1]);
	leaf.pos.y = uintBitsToFloat(l_leaves[leafIndexInArray + 2]);
	leaf.pos.z = uintBitsToFloat(l_leaves[leafIndexInArray + 3]);
	poly.degree = l_leaves[leafIndexInArray + 4];

	poly.coeffCount = getCoeffCount(poly.degree);

	for (int i = 0; i < poly.coeffCount; i += 2)
	{
		// packHalf2x16();
		vec2 coeffs = unpackSnorm2x16(l_leaves[leafIndexInArray + 5 + i / 2]);
		poly.coeffs[i + 0] = coeffs.x * coeffCompressAmount;
		poly.coeffs[i + 1] = coeffs.y * coeffCompressAmount;
	}

	leaf.poly = poly;

	return leaf;
}

Leaf searchForLeaf(vec3 p, out vec3 boxMin, out vec3 boxMax)
{
#ifdef USE_LOOKUP_TABLE
	ivec3 posInGrid = clamp(ivec3(floor(p * lookupTableSize)), ivec3(0), ivec3(lookupTableSize - 1));
	uint pointerToLeaf = texelFetch(lookupTable, posInGrid, 0).x;

	Leaf l = getLeaf(pointerToLeaf);

	boxMin = l.pos;
	boxMax = boxMin + vec3(1.0f / (1 << l.level));

	return l;
#else

	vec3 localP = p;
	// last element is the root
	uint currentBranchId = branchCount - 1;

	boxMin = vec3(0);
	boxMax = vec3(1);

	while (currentBranchId < branchCount)
	{
		Branch br = getBranch(currentBranchId);		

		vec3 halfBoxSize = vec3(1.0f / float( 1 << ( 1 + getBranchLevel( currentBranchId ) ) ) );
		// int pointerId = int(localP.z >= 0.5f) * 4 + int(localP.y >= 0.5f) * 2 + int(localP.x >= 0.5f);
		int pointerId = 0;

		// clamp to [0,1] may be free on certain architectures and otherwise very fast; usually, ternary provokes
		// a conditional move instead of an actual branch, so if you have lots of branching, it is worth a shot
		// to rephrase them into conditional moves and wave-coherent operations
		int xG = ( localP.x > 0.5f ) ? 1 : 0;
		int yG = ( localP.y > 0.5f ) ? 1 : 0;
		int zG = ( localP.z > 0.5f ) ? 1 : 0;

		pointerId += xG + 2 * yG + 4 * zG;
		localP -= 0.5f * vec3( xG, yG, zG );

		boxMin += vec3( xG, yG, zG ) * halfBoxSize;
		boxMax += ( vec3( xG, yG, zG ) - 1 ) * halfBoxSize;

		localP *= 2.0f;
		currentBranchId = br.pointers[pointerId];
	}

	return getLeaf(currentBranchId - branchCount);
#endif
}
