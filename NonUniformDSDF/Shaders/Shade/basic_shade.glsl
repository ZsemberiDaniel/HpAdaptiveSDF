//?#version 450
//?#include "../common.glsl"
float ambient_occlusion_basic(vec3 p, vec3 n);
//?#include "ambient_occlusion.glsl"
vec3 cook_torrance(vec3 V, vec3 N, vec3 L, Material mat);
//?#include "brdf.glsl"


uniform vec3 gLightPos;
const vec3 gNormEps = vec3(0.001);

vec3 smart_normalize( vec3 v )
{
	float v_length_sq = dot( v, v );
	// reciprocal square root; unless you have a precise qualifier, your
	// divisions will end up being rcp AMD ISA calls anyway, so using
	// the rsqrt won't be much of a difference in terms of quality -
	// but it will be less cycles.
	return ( v_length_sq != 0 ) ? v * inversesqrt( v_length_sq ) : 0.0.xxx;
}

vec3 norm_2(vec3 p)
{
	vec4 e0 = vec4(gNormEps, 0);
	vec3 plus = vec3(sdf(p + e0.xww),sdf(p + e0.wyw),sdf(p + e0.wwz));
	vec3 mins = vec3(sdf(p - e0.xww),sdf(p - e0.wyw),sdf(p - e0.wwz));
	vec3 diff = plus - mins;
	return smart_normalize( diff );
}

vec3 shade(RayCone raycone, float t, Material mat)
{
	vec3 p = RAY(raycone.ray, t);
	vec3 n = norm_2(p);
	vec3 retcol = vec3(0);
	retcol += cook_torrance(-raycone.ray.Direction, n, normalize(gLightPos - p), mat);
	float occlusion = ambient_occlusion_basic(p, n);
	retcol += mat.ambient * occlusion;
	// if(isnan(occlusion)) return vec3(1,0,0); // debug
	// return vec3(occlusion); // debug
	return retcol;
}
