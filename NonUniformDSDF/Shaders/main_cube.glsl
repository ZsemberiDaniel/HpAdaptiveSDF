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

uniform float time;
uniform float epsilon = 0.002f;
uniform float smallestStep;
uniform float biggestStep;
uniform float stepMultiplier;

// 0 - basic with refine root
// 1 - relaxed
// 2 - enhanced
uniform int sphereTraceType = 0;

//!#include "common.glsl"
//?#include "Math/box_ray_intersection.glsl"
//?#include "Shade/basic_shade.glsl"

TraceResult doSphereTrace(RayCone cone, SphereTraceDesc params);

uniform float octreeSize;
float sdfInside(vec3 p)
{
	vec3 texCoord = (p - sdfTexCorner) * oneOverSdfTexSize;
	return getSample(texCoord);
}

// TODO: delete these
vec3 debug1(vec3 p);
vec3 debugDist = vec3(1,0,0);

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
	// Box box = Box(modelTrans + 0.5 * modelScale, 0.5 * modelScale);
	Box box = Box(sdfTexCorner + 0.5 * sdfTexSize, 0.5 * sdfTexSize);
	Ray ray = Ray(cone.ray.Origin, cone.ray.Direction);

	if (!intersectBox(box, ray, false, temp_t))
	{
		return false;
	}

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
//	vec3 p_temp = cone.ray.Origin + (cone.ray.Tmin + 0.001f) * cone.ray.Direction;
//	vec3 deg1 = debug1((p_temp - sdfTexCorner) * oneOverSdfTexSize);
//	out_col = deg1;
//	return true;
	// DEBUG END
	SphereTraceDesc params = SphereTraceDesc(epsilon, maxStep);

	// Basic sphere trace
	TraceResult tret = doSphereTrace(cone, params);

	// Shading from now on
	Material mat;
	mat.ambient		= gAmbient;
	mat.diffcol		= gDiffuse;
	mat.roughness	= gCookRoughness;
	mat.ior			= gCookIOR;

	vec3 col = shade(cone, tret.T, mat);	//Gabor's tone mapping:
	col = max(0.0.rrr, col - 0.004.rrr);
	col = (col * (6.2f * col + 0.5f)) / (col * (6.2f * col + 1.7f) + 0.06f);

//	if(debugDist.x < 0) debugDist = vec3(-debugDist.x, 0, 0);
//	else debugDist = vec3(0,debugDist.x, 0);
	// col = 10*debugDist;

	// NORMAL VECTOR
#ifdef SHOW_NORMALS
	vec3 pp = cone.ray.Origin + tret.T * cone.ray.Direction;
	vec2 h = vec2(0.0001, 0);
	col = vec3(sdf(pp + h.xyy) - sdf(pp - h.xyy),
				sdf(pp + h.yxy) - sdf(pp - h.yxy),
				sdf(pp + h.yyx) - sdf(pp - h.yyx))/2/h.x;
	col = col * 0.5f + 0.5f;
#endif

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

	// out_col = vec3(sdfInside(cone.ray.Origin), 0, 0);

	return true;
}

void refineRoots( in RayCone cone, in float prevT, in float prevSign, inout float retT, inout float dd )
{
	if ( prevSign > 0 && dd < 0 )
	{
		if ( refineRoot == 1 )
		{
			// refine N times
			float a = prevT;
			float b = retT;
			float fa = prevSign;
			float fb = dd;

			for ( int j = 0; j < 10 && fa * fb < 0; ++j )
			{
				float h = ( a + b ) * 0.5;
				float fh = sdfInside( cone.ray.Origin + h * cone.ray.Direction );

				if ( fh < 0 )
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
			retT = a;
			dd = fa;
		}
		else if ( refineRoot == 2 )
		{
			float f0 = prevSign;
			float f1 = dd;
			float t = ( f0 - f1 == 0.0 ) ? 1.0 : f0 / ( f0 - f1 );
			retT = mix( prevT, retT, t );
			dd = mix( f0, f1, t );
		}
	}
}

#define STEP_SIZE_REDUCTION 0.95
TraceResult enhancedSphereTrace(in RayCone cone, SphereTraceDesc params)
{
	TraceResult ret = TraceResult(cone.ray.Tmin, 0);
	float rp = 0, rc = 0, rn = 0; //prev, curr, next
	float di = 0;
	int i = 0;
	do {
		di = rc + STEP_SIZE_REDUCTION * rc * max( (di - rp + rc) / (di + rp - rc), 0.6);
		rn = sdfInside(cone.ray.Origin + cone.ray.Direction * (ret.T + di));

		if(di > rc + rn)
		{
			di = rc;
			rn = sdfInside(cone.ray.Origin + cone.ray.Direction * (ret.T + di));
		}
		di = clamp(stepMultiplier * di, smallestStep, biggestStep);
		ret.T += di;
		rp = rc; rc = rn;
		++i;
	} while (
		ret.T < cone.ray.Tmax &&       			// Stay within bound box
		rn	  > params.epsilon * ret.T &&	// Stop if cone is close to surface
		i     < params.maxiters);

	// TODO: fix signature
	//refineRoots( cone, prevT, prevSign, ret.T, dd );

	ret.flags =  int(ret.T >= cone.ray.Tmax)
              | (int(rn <= params.epsilon)  << 1)
              | (int(i >= params.maxiters) << 2); 

	return ret;
}

#define TRACE_OR 1.6
TraceResult relaxedSphereTracing(in RayCone cone, in SphereTraceDesc params)
{
	TraceResult ret = TraceResult(cone.ray.Tmin, 0);

	float rc = 0, rn;
	float di = 0;
	int i = 0;
	do
	{
		di = rc * TRACE_OR;
		rn = sdfInside(cone.ray.Origin + cone.ray.Direction * (ret.T + di)); 
		if(di > rc + rn)
		{
			di = rc;
			rn = sdfInside(cone.ray.Origin + cone.ray.Direction * (ret.T + di));
		}
		di = stepMultiplier * di; // clamp( stepMultiplier * di, smallestStep, biggestStep );
		ret.T += di;
		rc = rn;
		++i;
	} while (
		ret.T < cone.ray.Tmax &&       					// Stay within bound box
		rn	  > params.epsilon * (ret.T + di) &&	// Stop if cone is close to surface
		i     < params.maxiters);

	// TODO: fix signature
	//refineRoots( cone, prevT, prevSign, ret.T, dd );

	ret.flags =  int(ret.T >= cone.ray.Tmax)
              | (int(di <= params.epsilon * (ret.T + di))  << 1)
              | (int(i >= params.maxiters) << 2);

	return ret;
}

TraceResult basicSphereTrace(in RayCone cone, in SphereTraceDesc params)
{
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
		dd = clamp(stepMultiplier * dd, smallestStep, biggestStep);
		debugDist = dd.xxx;
	}

	refineRoots( cone, prevT, prevSign, ret.T, dd );

	ret.flags = int(ret.T >= cone.ray.Tmax)
			  | (int(abs(dd) <= params.epsilon) << 1)
			  | (int(i >= params.maxiters) << 2);

	// restore global coordinates
	// ret.T *= sScale;
	return ret;
}


TraceResult doSphereTrace(in RayCone cone, SphereTraceDesc params)
{
	// trace in local model coordinates
//	cone.ray.Origin -= sTranslation + sdfTexCorner;
//	cone.ray.Origin /= sScale;
//	cone.ray.Tmin /= sScale;
//	cone.ray.Tmax /= sScale;
	if (sphereTraceType == 0)
	{
		return basicSphereTrace(cone, params);
	}
	else if (sphereTraceType == 1)
	{
		return relaxedSphereTracing(cone, params);
	}
	else if (sphereTraceType == 2)
	{
		return enhancedSphereTrace(cone, params);
	}
	else
	{
		return basicSphereTrace(cone, params);
	}
}
