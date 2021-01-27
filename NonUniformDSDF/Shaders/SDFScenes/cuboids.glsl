//?#include "../Math/primitives.glsl"

mat4 rotateX(float theta){
	float sintheta = sin(theta);
	float costheta = cos(theta);
	return mat4(vec4(1.0,0.0,0.0,0),
				vec4(0.0,costheta,-sintheta,0),
				vec4(0.0,sintheta,costheta,0),
				vec4(0.0,0.0,0.0,1.0));
}

mat4 rotateY(float theta){
	float sintheta = sin(theta);
	float costheta = cos(theta);
	return mat4(vec4(costheta,0.0,sintheta,0),
				vec4(0.0,1.0,0.0,0),
				vec4(-sintheta,0.0,costheta,0),
				vec4(0.0,0.0,0.0,1.0));
}

mat4 rotateZ(float theta){
	float sintheta = sin(theta);
	float costheta = cos(theta);
	return mat4(vec4(costheta,-sintheta,0.0,0),
				vec4(sintheta,costheta,0.0,0),
				vec4(0.0,0.0,1.0,0),
				vec4(0.0,0.0,0.0,1.0));
}
float opUnion(float dist1, float dist2){
	return min(dist1,dist2);
}

float funDist(vec3 p)
{
	float distances[4];
	int n = 0;
	vec4 p4 = vec4(p,1);
	float currentscale = 1.0;
	p4 = p4-vec4(vec3(0.400000, 0.200000, 0.000000),0);
	p4 = rotateZ(5.235988)*p4;
	p4 = rotateX(5.677556)*p4;
	distances[n] = box(p4.xyz,vec3(0.400000,0.150000,0.200000));
	n++;
	p4 = rotateX(0.605629)*p4;
	p4 = rotateZ(1.047198)*p4;
	p4 = p4+vec4(vec3(0.400000, 0.200000, 0.000000),0);
	p4 = p4-vec4(vec3(-0.200000, -0.300000, -0.300000),0);
	distances[n] = box(p4.xyz,vec3(0.300000,0.200000,0.500000));
	n++;
	p4 = p4+vec4(vec3(-0.200000, -0.300000, -0.300000),0);
	distances[n-2] = opUnion(distances[n-2],distances[n-1]);
	n--;
	p4 = p4-vec4(vec3(0.100000, -0.200000, 0.000000),0);
	distances[n] = box(p4.xyz,vec3(0.600000,0.300000,0.200000));
	n++;
	p4 = p4+vec4(vec3(0.100000, -0.200000, 0.000000),0);
	p4 = p4-vec4(vec3(0.000000, 0.100000, -0.800000),0);
	distances[n] = box(p4.xyz,vec3(0.200000,0.400000,0.200000));
	n++;
	p4 = p4+vec4(vec3(0.000000, 0.100000, -0.800000),0);
	distances[n-2] = opUnion(distances[n-2],distances[n-1]);
	n--;
	distances[n-2] = opUnion(distances[n-2],distances[n-1]);
	n--;
	return distances[n-1];
}
