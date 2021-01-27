//?#include "../Math/primitives.glsl"

// https://www.shadertoy.com/view/ltfSWn
// bounding box corner: ( ), size : ( )
float mandelbulb( vec3 p )
{
	vec3 w = p;
	float m = dot(w,w);

	float dz = 1.0;
	
	
	for( int i=0; i<4; i++ )
	{
#if 0
		float m2 = m*m;
		float m4 = m2*m2;
		dz = 8.0*sqrt(m4*m2*m)*dz + 1.0;

		float x = w.x; float x2 = x*x; float x4 = x2*x2;
		float y = w.y; float y2 = y*y; float y4 = y2*y2;
		float z = w.z; float z2 = z*z; float z4 = z2*z2;

		float k3 = x2 + z2;
		float k2 = inversesqrt( k3*k3*k3*k3*k3*k3*k3 );
		float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
		float k4 = x2 - y2 + z2;

		w.x = p.x +  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
		w.y = p.y + -16.0*y2*k3*k4*k4 + k1*k1;
		w.z = p.z +  -8.0*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;
#else
		dz = 8.0*pow(sqrt(m),7.0)*dz + 1.0;
		//dz = 8.0*pow(m,3.5)*dz + 1.0;
		
		float r = length(w);
		float b = 8.0*acos( w.y/r);
		float a = 8.0*atan( w.x, w.z );
		w = p + pow(r,8.0) * vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );
#endif

		m = dot(w,w);
		if( m > 256.0 )
			break;
	}

	return 0.25*log(m)*sqrt(m)/dz;
}

float funDist(vec3 p)
{
	// mandelbulb
	return mandelbulb(p);
}
