//?#version 450

// const int v1[24] = int[24](0,1,4,0, 3,0,3,6, 0,3,6,0, 2,0,2,5, 0,2,5,0, 1,0,1,4);
// const int v2[24] = int[24](1,4,7,1, 4,3,6,7, 3,6,7,1, 6,2,5,7, 2,5,7,1, 5,1,4,7);
//      0,1,1,4,4,7,0,1      000 001 001 100 100 111 000 001  ->  001 000 111 100 100 001 001 000  =  0x23C848
//      3,4,0,3,3,6,6,7      011 100 000 011 011 110 110 111  ->  111 110 110 011 011 000 100 011  =  0xFB3623
//      0,3,3,6,6,7,0,1      000 011 011 110 110 111 000 001  ->  001 000 111 110 110 011 011 000  =  0x23ECD8
//      2,6,0,2,2,5,5,7      010 110 000 010 010 101 101 111  ->  111 101 101 010 010 000 110 010  =  0xF6A432
//      0,2,2,5,5,7,0,1      000 010 010 101 101 111 000 001  ->  001 000 111 101 101 010 010 000  =  0x23DA90
//      1,5,0,1,1,4,4,7      001 101 000 001 001 100 100 111  ->  111 100 100 001 001 000 101 001  =  0xF21229
const int Edges[6] = int[6](0x23C848, 0xFB3623, 0x23ECD8, 0xF6A432, 0x23DA90, 0xF21229);
//	const int nSeq_[64] = int[64](//                         flip
//		0,1,4,2,3,5,6,7,  // 000 001 100 010 011 101 110 111  ->  111 110 101 011 010 100 001 000  =  0xFAB508
//		1,5,0,3,7,4,2,6,  // 001 101 000 011 111 100 010 110  ->  110 010 100 111 011 000 101 001  =  0xCA7629
//		2,3,0,6,7,1,4,5,  // 010 011 000 110 111 001 100 101  ->  101 100 001 111 110 000 011 010  =  0xB0FC1A
//		3,1,2,7,5,0,6,4,  // 011 001 010 111 101 000 110 100  ->  100 110 000 101 111 010 001 011  =  0x985E8B
//		4,5,6,0,1,7,2,3,  // 100 101 110 000 001 111 010 011  ->  011 010 111 001 000 110 101 100  =  0x6B91AC
//		5,7,4,1,3,6,0,2,  // 101 111 100 001 011 110 000 010  ->  010 000 110 011 001 100 111 101  =  0x43333D
//		6,7,2,4,5,3,0,1,  // 110 111 010 100 101 011 000 001  ->  001 000 011 101 100 010 111 110  =  0x21D8BE
//		7,3,6,5,1,2,4,0); // 111 011 110 101 001 010 100 000  ->  000 100 010 001 101 110 011 111  =  0x111B9F
const int nSeq[8] = int[8](0xFAB508, 0xCA7629, 0xB0FC1A, 0x985E8B, 0x6B91AC, 0x43333D, 0x21D8BE, 0x111B9F);

int getFrontIndex(vec3 dir)
{
	return (dir.x < 0 ? 4 : 0) + (dir.y < 0 ? 2 : 0) + (dir.z < 0 ? 1 : 0);
}
int getEdge(int vertID, int edgeNum, int end)
{
	return (Edges[vertID] >> (6*edgeNum + 3*end)) & 7;
	// int ind = 4 * vertID + edgeNum;
	// return end == 0 ? v1[ind] : v2[ind];
}
int getSeq(int frontID, int id)
{
	return (nSeq[frontID] >> (3*id)) & 7;
	// return nSeq_[8*frontID + id];
}
vec3 getCubeVertex(int id)
{
	return vec3((id & 4) != 0 ? 1 : 0, (id & 2) != 0 ? 1 : 0, (id & 1) != 0 ? 1 : 0);
}

// [0,1]^3 unit cube scaled then translated
struct BBox { vec3 size; vec3 corner; };
// plane with equation <norm, x> = dist
struct Plane{ vec3 norm; float dist; };

// box_plane_intersection
// triangle_fan with 6 vertices
// based on https://www.researchgate.net/profile/Andreas_Kolb2/publication/215504964_A_Vertex_Program_for_Efficient_Box-Plane_Intersection/links/568a955408ae1e63f1fbd895/A-Vertex-Program-for-Efficient-Box-Plane-Intersection.pdf
vec3 box_plane_intersection(int vert_id, BBox box, Plane plane)
{
	int frontIndex = getFrontIndex(plane.norm);
	for(int i = 0; i < 4; ++i)
	{
		int vid1 = getSeq(frontIndex, getEdge(vert_id, i, 0));
		int vid2 = getSeq(frontIndex, getEdge(vert_id, i, 1));
		vec3 V1 = getCubeVertex(vid1) * box.size;
		vec3 V2 = getCubeVertex(vid2) * box.size;
		vec3 start = V1 + box.corner;
		vec3 dir = V2 - V1;
		float denom = dot(dir, plane.norm);
		float lambda = (denom != 0) ? (plane.dist - dot(start, plane.norm))/denom : -1;
		if((lambda >= 0) && (lambda <= 1)){
			return start + lambda * dir;
		}
	}
	return vec3(0,0,0);
}
