//?#version 450

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

const int MAX_COEFF_SIZE = 286; // deg 10
struct Polynom {
	uint degree;
	uint coeffCount;
	float coeffs[MAX_COEFF_SIZE];
};

float evalPolynom(in Polynom poly, vec3 p)
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

uint getCoeffCount(uint degree) 
{
	return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
}
