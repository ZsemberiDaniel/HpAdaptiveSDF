//?#include "../common.glsl"

// sphere: [center.xyz, radius]
float intersectSphere(vec4 sphere, Ray ray)
{
	vec3 s = ray.origin - sphere.xyz;
	vec3 v = ray.dir;
	float r = sphere.w;
	float A = dot(v, v);
	float B = 2 * dot(s, v);
	float C = dot(s, s) - r * r;
	float D = B * B - 4 * A*C;
	if (D < 0) return -1;
	return (-B - sqrt(D)) / (2 * A);
}