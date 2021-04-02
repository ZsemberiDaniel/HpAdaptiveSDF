//?#version 450

//?#include "../defines.glsl"

float legendref(int deg, float p)
{
	if (deg == 0) return 1;
	else if (deg == 1) return p;

	float a = 1;
	float b = p;

	for (int at = 2; at <= deg; at++)
	{
		float c = (1.0f / at) * ((2.0f * at - 1.0f) * p * b - (at - 1.0f) * a);
		a = b; b = c;
	}

	return b;
}

struct Polynom {
	uint degree;
	uint coeffCount;
	float coeffs[150];
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
