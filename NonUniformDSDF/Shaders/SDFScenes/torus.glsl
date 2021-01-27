//?#include "../Math/primitives.glsl"

float torus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xy) - t.x, p.z);
    return length(q) - t.y;
}

float funDist(vec3 p)
{
    // torus
    return torus(p-param1, param2.xy);
}
