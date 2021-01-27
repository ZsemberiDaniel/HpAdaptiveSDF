//?#version 450
//?#include "../common.glsl"

float maxx(vec3 v) { return max (max(v.x, v.y), v.z); }

// special stripped down version of the function on the bottom
bool intersectBox(Box box, Ray ray, bool front, out float dist) {
	ray.origin = ray.origin - box.center;
	float winding = front ? 1 : -1;
	vec3 sgn = -sign(ray.dir);
	vec3 d = box.radius * winding * sgn - ray.origin;
	d /= ray.dir;
# define TEST_BOX(U, VW) (d.U >= 0.0) && all(lessThan(abs(ray.origin.VW + ray.dir.VW * d.U), box.radius.VW))
	bvec3 test = bvec3(TEST_BOX(x, yz), TEST_BOX(y, zx), TEST_BOX(z, xy));
# undef TEST_BOX
	sgn = test.x ? vec3(sgn.x,0,0) : (test.y ? vec3(0,sgn.y,0) : vec3(0,0,test.z ? sgn.z:0));
	dist = (sgn.x != 0) ? d.x : ((sgn.y != 0) ? d.y : d.z);
	return (sgn.x != 0) || (sgn.y != 0) || (sgn.z != 0);
}

//// http://www.jcgt.org/published/0007/03/04/paper-lowres.pdf
//// box.rotation = object-to-world, invRayDir unused if oriented
///
//bool ourIntersectBox(Box box, Ray ray, out float dist, out vec3 normal,
//	const bool canStartInBox, /*const in bool oriented,*/ in vec3 _invRayDir) {
//
//	ray.origin = ray.origin - box.center; // box az origóba kerül
////	if (oriented) { ray.dir *= box.rot; ray.origin *= box.rot; }
//
//	float winding = canStartInBox && (maxx(abs(ray.origin) * box.invRadius) < 1.0) ? -1 : 1; // are we inside?
//	vec3 sgn = -sign(ray.dir); // ray "direction"
//	// Distance to plane
//	vec3 d = box.radius * winding * sgn - ray.origin;
////	if (oriented) d /= ray.dir; else
//	d *= _invRayDir;
//# define TEST(U, VW) (d.U >= 0.0) && all(lessThan(abs(ray.origin.VW + ray.dir.VW * d.U), box.radius.VW))
//	bvec3 test = bvec3(TEST(x, yz), TEST(y, zx), TEST(z, xy));
//# undef TEST
//	sgn = test.x ? vec3(sgn.x,0,0) : (test.y ? vec3(0,sgn.y,0) : vec3(0,0,test.z ? sgn.z:0));
//	dist = (sgn.x != 0) ? d.x : ((sgn.y != 0) ? d.y : d.z);
//	normal =
////	oriented ? (box.rot * sgn) :
//	sgn;
//	return (sgn.x != 0) || (sgn.y != 0) || (sgn.z != 0);
//}
//
