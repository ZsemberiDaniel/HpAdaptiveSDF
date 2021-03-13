#version 450

out vec4 fs_out_col;

uniform vec4 color;

void main()
{
	fs_out_col = color;
}

