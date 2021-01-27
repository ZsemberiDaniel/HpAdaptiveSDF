//?#include "../Math/primitives.glsl"

// https://www.shadertoy.com/view/wlj3zV
// bounding box corner: (-.5, -1, -1.6), size : (2, 2, 2)
float sdSqCappedTorus(in vec3 p, in vec2 sc, in float ra)
{
    p.x = abs(p.x);
    float k = (sc.y*p.x>sc.x*p.y) ? dot(p.xy,sc) : length(p.xy);
    return dot(p,p) + ra*ra - 2.0*ra*k;
}
vec3 hashTorus( float n )
{
    vec3 m = n*vec3(23.0,41.0,17.0) + vec3(9.0,1.0,31.0);
    return fract( m*fract( m*0.3183099 ) );
}
vec2 torusPipes( in vec3 pos )
{
    vec3 pp = vec3(0.0);
    vec3 ww = vec3(0.0,1.0,0.0);
    
    float d = length(pos-pp);
    
    vec4 data = vec4(0.0);
                   
    for( int i=0; i<32; i++ )
    {
        // segment parameters        
        vec3 ran = hashTorus(float(i));
        float ra = 0.13 + 0.08*ran.x; // radius
        float ap = 1.10 + 0.80*ran.y; // aperture
        vec3  up = normalize( sin(75.0*ran.z+vec3(0.0,1.0,4.0))); // orientation

        // world to torus transformation
        vec2 sc = vec2(sin(ap),cos(ap));
        vec3 ou = normalize(cross(ww,up));
        vec3 vv = cross(ou,ww);
        vec3 uu =  sc.x*ou + sc.y*ww;
             ww = -sc.y*ou + sc.x*ww;
        vec3 cpos = (pos-pp)*mat3(uu,ww,vv) + ra*vec3(-sc.x,sc.y,0.0);
        
        // distance evaluation        
        float tmp = sdSqCappedTorus(cpos, sc, ra );
        if( tmp<d )
        {
            d = tmp;
            data = vec4( float(i), cpos.xy, ap );
        }
        
        // prepare next segment        
        pp += 2.0*ra*sc.x*uu;
        ww = sc.y*uu - sc.x*ww;
    }
    
    return vec2(sqrt(d) - 0.035, // distance
                data.x + 0.5+0.5*(atan(data.y,data.z))/data.w // u
               );
}

float funDist(vec3 p)
{
    // torus pipes
    return torusPipes(p - param1).x;
}
