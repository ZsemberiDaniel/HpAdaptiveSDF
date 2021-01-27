#version 450

out vec4 fs_out_col;

uniform vec3 color;

void main()
{
	fs_out_col = vec4(color, 1.0);
}

