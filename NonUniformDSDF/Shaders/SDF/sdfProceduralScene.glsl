//?#include "../common.glsl"

float funDist(vec3 p); // from "../SDFScenes/*.glsl"

float getSample(vec3 x)
{
	return funDist(x*sdfTexSize + sdfTexCorner);
}