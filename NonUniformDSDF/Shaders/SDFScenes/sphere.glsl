//?#include "../Math/primitives.glsl"

float funDist(vec3 p)
{
	// sphere
	return length(p - param1) - param2.x;

	// sphere + cube
	//return min(box(p-vec3(0.3), vec3(0.2)), length(p-vec3(0.7, 0.3, 0.3))-0.2);

	// plane
	//return plane(p-param1, param2);
}
