//?#include "../Math/primitives.glsl"

float funDist(vec3 p)
{
	return box(p-vec3(0.3), vec3(0.5));
}
