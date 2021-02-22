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

float evalPolynom(float[] coeffs, uint deg, vec3 p)
{
	float ret = 0.0f;

	vec3 pPower = vec3(1);
	for (int i = 0, m = 0; i <= deg; i++)
	{
		pPower.y = 1.0f;
		for (int k = 0; i + k <= deg; k++)
		{
			pPower.z = 1.0f;
			for (int j = 0; i + k + j <= deg; j++, m++)
			{
				ret += coeffs[m] * pPower.x * pPower.y * pPower.z;

				pPower.z *= p.z;
			}
			pPower.y *= p.y;
		}
		pPower.x *= p.x;
	}

	return ret;
}

uint getCoeffCount(uint degree) 
{
	return (degree * degree * degree + 6 * degree * degree + 11 * degree + 6) / 6;
}
