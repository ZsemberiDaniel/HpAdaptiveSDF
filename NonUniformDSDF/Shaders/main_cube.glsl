#version 450

uniform vec3 gCameraPos;
uniform float gTanPixelAngle;

//uniform vec3 gCameraDir;

uniform vec3 gAmbient;
uniform vec3 gDiffuse;
uniform vec3 gCookRoughness;
uniform vec3 gCookIOR;

uniform int refineRoot;

uniform int maxStep = 100;

//!#include "common.glsl"
//?#include "Math/box_ray_intersection.glsl"
//?#include "Shade/basic_shade.glsl"

TraceResult basicSphereTrace(RayCone cone, SphereTraceDesc params);

float sdfInside(vec3 p)
{
	vec3 texCoord = p / sdfTexSize;
	return getSample(texCoord);
}

// TODO: delete these
vec3 debug1(vec3 p);

// returns false if the fragment should be discarded (the cube was not hit by the ray)
// in_vec: camera direction or fragment position in world coordinates (see pos_or_dir)
// pos_or_dir: if true in_vec is interpreted as a position, if false in_vec is a direction
// calc_t_min: calculate ray.Tmin; if in_vec is a direction this has no effect (and Tmin will be calculated)
bool cube_main(vec3 in_vec, bool pos_or_dir, bool calc_t_min, out vec3 out_col, out float out_depth)
{
	vec3 worldVec = in_vec;
	if(pos_or_dir)
		worldVec = worldVec - gCameraPos;

	// constructing camera ray
	float wVecLen = length(worldVec);
	RayCone cone;
	cone.ray.Direction	= worldVec / wVecLen;
	cone.ray.Origin		= gCameraPos;
	cone.tana			= gTanPixelAngle;
	cone.rad			= 0.0f;
	cone.ray.Tmin		= wVecLen;

	// construct box in which the sdf is
	float temp_t = 0.0f;
	Box box = Box(modelTrans + 0.5 * modelScale, 0.5 * modelScale);
	Ray ray = Ray(cone.ray.Origin, cone.ray.Direction);
	if(!intersectBox(box, ray, false, temp_t))
		return false;

	// intersectBox returned the maximum possible t inside box
	cone.ray.Tmax = temp_t;

	// t min calculated
	if(calc_t_min || !pos_or_dir){
		intersectBox(box, ray, true, temp_t);
		cone.ray.Tmin = max(cone.ray.Tmin, temp_t);
	}
	if(cone.ray.Tmax <= cone.ray.Tmin)
		return false;
		
	
	// DEBUG START
//	cone.ray.Origin -= sTranslation + sdfTexCorner;
//	cone.ray.Origin /= sScale;
//	cone.ray.Tmin /= sScale;
//	cone.ray.Tmax /= sScale;
//	vec3 p_temp = cone.ray.Origin + (cone.ray.Tmin + 0.01f) * cone.ray.Direction;
//	vec3 deg1 = debug1(p_temp / sdfTexSize);
//	out_col = deg1;
//	return true;
	// DEBUG END
	SphereTraceDesc params = SphereTraceDesc(0.002, maxStep);

	// Basic sphere trace
	TraceResult tret = basicSphereTrace(cone, params);

	// Shading from now on
	Material mat;
	mat.ambient		= gAmbient;
	mat.diffcol		= gDiffuse;
	mat.roughness	= gCookRoughness;
	mat.ior			= gCookIOR;

	vec3 col = shade(cone, tret.T, mat);	//Gabor's tone mapping:
	col = max(0.0.rrr, col - 0.004.rrr);
	col = (col * (6.2f * col + 0.5f)) / (col * (6.2f * col + 1.7f) + 0.06f);

	if(bool(tret.flags & (1<<0))){
		// tmax reached
		return false;
	}else if(bool(tret.flags &(1<<2))){
		// max iterations reached
		col = vec3(1,0,1);
	}
	out_col = col;

	vec4 depth_vec = viewProj * vec4(tret.T*cone.ray.Direction + cone.ray.Origin, 1);
	out_depth = ((depth_vec.z / depth_vec.w) + 1.0) * 0.5;

	return true;
}

TraceResult basicSphereTrace(RayCone cone, SphereTraceDesc params)
{
	// trace in local model coordinates
	cone.ray.Origin -= sTranslation + sdfTexCorner;
	cone.ray.Origin /= sScale;
	cone.ray.Tmin /= sScale;
	cone.ray.Tmax /= sScale;

	TraceResult ret = TraceResult(cone.ray.Tmin, 0);

	int i = 0;
	float dd = sdfInside(cone.ray.Origin + ret.T * cone.ray.Direction);
	float prevSign = dd;
	float prevT = ret.T;
	for (; i < params.maxiters && dd > params.epsilon && ret.T < cone.ray.Tmax; ++i)
	{
		prevT = ret.T;
		ret.T += dd;
		prevSign = dd;
		dd = sdfInside(cone.ray.Origin + ret.T * cone.ray.Direction);
	}

	if (refineRoot == 1 && prevSign > 0 && dd < 0)
	{
		// refine N times
		float a = prevT;
		float b = ret.T;
		float fa = prevSign;
		float fb = dd;

		for (int j = 0; j < 10 && fa*fb < 0; ++j)
		{
			float h = (a + b)*0.5;
			float fh = sdfInside(cone.ray.Origin + h * cone.ray.Direction);

			if (fh < 0)
			{
				// replace right endpoint
				b = h;
				fb = fh;
			}
			else
			{
				// replace left endpoint
				a = h;
				fa = fh;
			}
		}
		ret.T = a;
		dd = fa;
	}

	ret.flags = int(ret.T >= cone.ray.Tmax)
			  | (int(abs(dd) <= params.epsilon) << 1)
			  | (int(i >= params.maxiters) << 2);

	// restore global coordinates
	ret.T *= sScale;
	return ret;

}