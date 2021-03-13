#version 450

uint[6] I = uint[6](
	0, 1, 2,
	0, 2, 3
);

uniform vec3 p1;
uniform vec3 p2;
uniform vec3 p3;
uniform vec3 p4;
uniform mat4 MVP;

void main()
{
	vec3 ps[4] = {p1, p2, p3, p4};

	vec3 pos = ps[I[gl_VertexID]];
	gl_Position = MVP * vec4( pos, 1 );
}
