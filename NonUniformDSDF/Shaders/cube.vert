#version 450

vec3[8] P = vec3[8](
	vec3(0,0,0), vec3(0,0,1), vec3(0,1,0), vec3(0,1,1),
	vec3(1,0,0), vec3(1,0,1), vec3(1,1,0), vec3(1,1,1)
);

uint[24] I = uint[24](
	0,1, 0,2, 0,4,
	1,3, 1,5,
	2,3, 2,6,
	3,7,
	4,5, 4,6,
	5,7,
	6,7
);

uniform mat4 MVP;

void main()
{
	vec3 pos = P[I[gl_VertexID]];
	gl_Position = MVP * vec4( pos, 1 );
}