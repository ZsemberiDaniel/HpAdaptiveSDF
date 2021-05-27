//?#version 450

//?#include "../defines.glsl"

float legendref(in int deg, float p)
{
	if (deg == 0) return 1;
	else if (deg == 1) return p;
	else if (deg == 2) return 0.5f * (3.0f * p * p - 1.0f);
	else if (deg == 3) return 0.5f * p * (5.0f * p * p - 3.0f);
	else if (deg == 4) return 0.125f * (p * p * (35.0f * p * p - 30.0f) + 3.0f);
	else if (deg == 5) return 0.125f * p * (p * p * (63.0f * p * p - 70.0f) + 15.0f);
	else if (deg == 6) 
	{
		float sq = p * p;
		return 0.0625f * (sq * (sq * (231.0f * sq - 315.0f) + 105.0f) - 5.0f);
	}
	else if (deg == 7) 
	{
		float sq = p * p;
		return 0.0625f * p * (sq * (sq * (429.0f * sq - 693.0f) + 315.0f) - 35.0f);
	}
	
	float sq = p * p;
	float a = 0.0625f * (sq * (sq * (231.0f * sq - 315.0f) + 105.0f) - 5.0f);
	float b = 0.0625f * p * (sq * (sq * (429.0f * sq - 693.0f) + 315.0f) - 35.0f);

	for (int at = 7; at <= deg; at++)
	{
		float c = (1.0f / at) * ((2.0f * at - 1.0f) * p * b - (at - 1.0f) * a);
		a = b; b = c;
	}

	return b;
}

struct Polynom {
	uint degree;
	uint coeffCount;
	float coeffs[MAX_COEFF_SIZE_IN_LEAVES];
};

float evalPolynom_normLagrange(in Polynom poly, vec3 p, vec3 bboxSize)
{
	float ret = 0.0f;

	for (int i = 0, m = 0; i <= poly.degree; i++)
	{
		for (int k = 0; i + k <= poly.degree; k++)
		{
			for (int j = 0; i + k + j <= poly.degree; j++, m++)
			{
				vec3 a = sqrt((2.0f * vec3(i, k, j) + 1.0f) / bboxSize);
				// a = vec3(1);
				ret += poly.coeffs[m] * legendref(i, p.x) * legendref(k, p.y) * legendref(j, p.z) *
					a.x * a.y * a.z;
			}
		}
	}

	return ret;
}

float evalPolynom_lagrange(in Polynom poly, vec3 p, vec3 bboxSize)
{
	float ret = 0.0f;

	for (int i = 0, m = 0; i <= poly.degree; i++)
	{
		for (int k = 0; i + k <= poly.degree; k++)
		{
			for (int j = 0; i + k + j <= poly.degree; j++, m++)
			{
				ret += poly.coeffs[m] * legendref(i, p.x) * legendref(k, p.y) * legendref(j, p.z);
			}
		}
	}

	return ret;
}

// unused
float evalPolynom_monomial(in Polynom poly, vec3 p, vec3 bboxSize)
{
	float ret = 0.0f;

	for (int i = 0, m = 0; i <= poly.degree; i++)
	{
		for (int k = 0; i + k <= poly.degree; k++)
		{
			for (int j = 0; i + k + j <= poly.degree; j++, m++)
			{
				ret += poly.coeffs[m] * pow(p.x, i) * pow(p.y, k) * pow(p.z, j);
			}
		}
	}

	return ret;
}

uint getCoeffCount(uint degree) 
{
	return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
}

int getCoeffCountI(int degree) 
{
	return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
}
