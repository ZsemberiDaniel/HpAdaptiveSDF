#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

layout(location = 0) in vec3 gs_in_pos[];
layout(location = 1) in vec3 gs_in_norm[];
layout(location = 2) in vec2 gs_in_tex[];

layout(location = 0) out vec3 gs_out_pos;
layout(location = 1) out vec3 gs_out_norm;
layout(location = 2) out vec2 gs_out_tex;

vec3 calcNorm()
{
   vec3 a = vec3(gs_in_pos[0]) - vec3(gs_in_pos[1]);
   vec3 b = vec3(gs_in_pos[2]) - vec3(gs_in_pos[1]);
   return normalize(cross(b, a));
}

void main()
{
	vec3 norm = calcNorm();

	for(int i = 0; i < 3;++i) {
		gl_Position = gl_in[i].gl_Position;
		gs_out_pos = gs_in_pos[i];
		gs_out_norm = norm;
		gs_out_tex = gs_in_tex[i];
		EmitVertex();
	}
}