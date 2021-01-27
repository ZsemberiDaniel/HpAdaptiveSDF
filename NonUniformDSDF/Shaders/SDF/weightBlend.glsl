
uniform int  blendFunction;

vec3 getBlendedWeight(vec3 r); // weightBlend.glsl

float CubicHermite(float t)
{
	return 3 * t*t - 2 * t*t*t;
}

float QuinticHermite(float t)
{
	return 10 * t*t*t - 15 * t*t*t*t + 6 * t*t*t*t*t;
}

uniform float Hartmann_Mu;
uniform int   Hartmann_n;
float Hartmann(float t)
{
	float TadNplusOne = pow(t, Hartmann_n + 1);
	float OneMinusTadNplusOne = pow(clamp(1.0 - t, 0.0, 1.0), Hartmann_n + 1);
	float Denom = Hartmann_Mu * TadNplusOne + (1.0 - Hartmann_Mu) * OneMinusTadNplusOne;

	// TODO: compute Hartmann's limit
	return Denom == 0 ? t : Hartmann_Mu * TadNplusOne / Denom;
}

// x: texture coordinates
vec3 getBlendedWeight(vec3 r)
{
	float ZWeight = r.z;
	float YWeight = r.y;
	float XWeight = r.x;

	if (blendFunction == 1)
	{
		ZWeight = CubicHermite(ZWeight);
		YWeight = CubicHermite(YWeight);
		XWeight = CubicHermite(XWeight);
	}
	else if (blendFunction == 2)
	{
		ZWeight = QuinticHermite(ZWeight);
		YWeight = QuinticHermite(YWeight);
		XWeight = QuinticHermite(XWeight);
	}
	else if (blendFunction == 3)
	{
		ZWeight = Hartmann(ZWeight);
		YWeight = Hartmann(YWeight);
		XWeight = Hartmann(XWeight);
	}

	return vec3(XWeight, YWeight, ZWeight);
}