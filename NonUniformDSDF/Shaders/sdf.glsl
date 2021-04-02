//?#version 450

//?#include "common.glsl"


//vec2 sampleSdfTex(vec3 p, vec3 dir);
//
//vec2 sdf(vec3 p, vec3 v)
//{
//	vec2 d = sScale * sampleSdfTex( (p - sTranslation - sdfTexCorner) * sOneOverScale + sdfTexCorner, v);
//	
//	return d;
//}
//not convex optimized version
float sdf(vec3 p) {
	return getSample((p - sdfTexCorner) / sdfTexSize);
	// return sdf(p,vec3(0,0,0)).x;
}
//
//vec3 vec2sdfTexInnerBox(vec3 p);
//float stepToSdfTexInnerBox(vec3 p, vec3 dir);
//vec2 sampleSdfTex(vec3 p, vec3 dir)
//{
//	vec3 v = vec2sdfTexInnerBox(p);
//	float vv = dot(v,v);
//	if(vv < 0.001){ // inside the inner box
//		vec3 x = (p - sdfTexCorner) / sdfTexSize;
//		float samp =  getSample(x);
//		return samp.xx;
//	}else{
//		float boxStep = stepToSdfTexInnerBox(p, dir);
//		float d = sqrt(vv);
//		return vec2(d, max(d,boxStep));
//	}
//}
//
//vec3 vec2sdfTexInnerBox(vec3 p)
//{
//	vec3 c = sdfTexCorner + sdfTexSize / 2;
//	vec3 pp = abs(p - c);
//	vec3 b = sdfTexSize / 2 - sdfTexBorder;
//	return max(vec3(0,0,0), pp - b);
//}
//
//float intersectBoundingBox(vec3 p, vec3 dir);
//
//// p is outside of the box
//float stepToSdfTexInnerBox(vec3 p, vec3 dir)
//{
//	if(dir == vec3(0,0,0)){ // no direction vector...
//		vec3 c = sdfTexCorner + sdfTexSize / 2;
//		vec3 pp = abs(p - c) - (sdfTexSize / 2 - sdfTexBorder);
//		return length(max(pp, 0.0));
//	}else{
//		return intersectBoundingBox(p, dir);
//	}
//}
//
//void swap(inout float a, inout float b){
//	float temp = a;
//	a = b;
//	b = temp;
//}
//
//float intersectBoundingBox(vec3 p, vec3 dir) 
//{ 
//	vec3 bmin = sdfTexCorner + .5 * sdfTexBorder;
//	vec3 bmax = sdfTexCorner + sdfTexSize - .5 * sdfTexBorder; 
//	float tmin = (bmin.x - p.x) / dir.x; 
//	float tmax = (bmax.x - p.x) / dir.x; 
// 
//	if (tmin > tmax) swap(tmin, tmax); 
// 
//	float tymin = (bmin.y - p.y) / dir.y; 
//	float tymax = (bmax.y - p.y) / dir.y; 
// 
//	if (tymin > tymax) swap(tymin, tymax); 
// 
//	if ((tmin > tymax) || (tymin > tmax)) 
//		return INF; 
// 
//	tmin = max(tmin, tymin);
//	tmax = min(tmax, tymax);
// 
//	float tzmin = (bmin.z - p.z) / dir.z; 
//	float tzmax = (bmax.z - p.z) / dir.z; 
// 
//	if (tzmin > tzmax) swap(tzmin, tzmax); 
// 
//	if ((tmin > tzmax) || (tzmin > tmax)) 
//		return INF; 
//	
//	tmin = max(tmin, tzmin);
//	tmax = min(tmax, tzmax);
// 
//	return tmin; 
//} 