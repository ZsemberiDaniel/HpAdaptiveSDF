#version 450

layout(location = 0) in vec3 fs_in_pos;
layout(location = 1) in vec3 fs_in_norm;
layout(location = 2) in vec2 fs_in_tex;

layout(location = 0) out vec4 fs_out_col;

uniform vec3 light_pos;
//uniform vec3 eye_pos;

uniform vec3 La = vec3(0.4, 0.4, 0.4);
uniform vec3 Ld = vec3(0.6, 0.6, 0.6);

//uniform sampler2D texImage;

void main()
{

	vec3 ambient = La;

	vec3 normal = normalize(fs_in_norm);
	vec3 to_light = normalize(light_pos - fs_in_pos);
	
	float cosa = clamp(dot(normal, to_light), 0, 1);

	vec3 diffuse = cosa*Ld;
	
	fs_out_col = vec4(ambient + diffuse, 1)/* * texture(texImage, fs_in_tex)*/;
}