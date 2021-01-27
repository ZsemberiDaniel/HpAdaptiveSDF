//?#version 450
#ifndef COMMON_GLSL_INCLUDED
#define COMMON_GLSL_INCLUDED

// common uniforms:
//!#include "uniforms.glsl"

#define SQRT2 1.4142135623
#define PI 3.14159265359
#define PI2 (2*3.14159265359)
#define INF 3.402823466e+38    //whats the maximum?

#define RAY(r,t) (r.Direction*(t)+r.Origin)
#define SDF(r,t) sdf(RAY(r,t))

struct RayDesc
{
	vec3 Origin;
	float Tmin;
	vec3 Direction;
	float Tmax;
};

struct RayCone
{
	RayDesc ray;    //Central axis of the cone
	float tana;		//Cone opening angle when cone tracing can be zero
	float rad;		//Cone starting radius at Origin (t=0)
};

struct TraceResult
{
	float T;		// Distance taken on ray
	int flags;		// bit 0:   distance condition:     true if travelled to far t > t_max
					// bit 1:   surface condition:      true if distance to surface is small < error threshold
					// bit 2:   iteration condition:    true if took too many iterations
//    int iterations;	// Iterations taken
//    float distance;   // Distance to the surface (lower bound)
};

struct SphereTraceDesc
{
	float epsilon;  //Cone stopping distance to surface
	int maxiters;   //Maximum iteration count
};

struct Box{
	vec3 center;
	vec3 radius;
//	vec3 invRadius;
//	mat3 rot;
};
struct Ray{
	vec3 origin;
	vec3 dir;
};

struct Material
{
	vec3 ambient;
	vec3 diffcol;
	vec3 roughness;
	vec3 ior;
};

float sdf(vec3 p); // in "sdf.glsl"
float getSample(vec3 x); // in "SDF/sdf*.glsl"

#endif
