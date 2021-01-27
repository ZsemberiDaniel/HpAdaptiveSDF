#version 450

//!#include "Math/box_plane_intersection.glsl"

layout(location = 0) out vec3 worldPos;

uniform vec3 modelScale;
uniform vec3 modelTrans;
//uniform mat4 model; // = translate(modelTrans) * scale(modelScale)
uniform mat4 viewProj;
uniform mat4 gInverseViewProj;

uniform vec3 gCameraPos;
uniform vec3 gCameraDir;
uniform float planeDist; // view plane distance from 0

// unit cube: triangle_strip with 14 vertices
// X: 10 1000 0111 1010  = 0x287a
// Y: 00 0010 1010 1111  = 0x2af
// Z: 11 0001 1110 0011  = 0x31e3
vec3 unit_cube(int vert_id)
{
	return vec3((ivec3(0x287a, 0x2af, 0x31e3) >> vert_id) & 1);
}

// full screen "quad": triangle with 3 vertices
vec4 full_screen_quad(int vert_id)
{
	const vec2 pos[3] = vec2[3](vec2(-1,-1), vec2( 3,-1), vec2(-1, 3));
	return vec4(pos[vert_id], -1, 1);
}

// gl_VertexID: 0..2 quad, 3..8 clip, 9..22 cube
void main()
{
	if (gl_VertexID >= 9) { // cube
		vec3 vert = unit_cube(gl_VertexID - 9);
		worldPos = vert * modelScale + modelTrans;
		gl_Position = viewProj * vec4(worldPos, 1);
	} else if (gl_VertexID >= 3) { // clip
		worldPos = box_plane_intersection(gl_VertexID - 3, BBox(modelScale, modelTrans), Plane(gCameraDir, planeDist));
		gl_Position = viewProj * vec4(worldPos, 1);
		gl_Position.z = -gl_Position.w;
	} else { // quad
		gl_Position = full_screen_quad(gl_VertexID);
		vec4 p = gInverseViewProj * gl_Position;
		worldPos = p.xyz / p.w;
	}
}
