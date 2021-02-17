#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 0) out vec4 fs_out_col;


bool cube_main(vec3 in_vec, bool pos_or_dir, bool calc_t_min, out vec3 out_col, out float out_depth);

void main()
{
	bool recalc_t_min = false;
#ifdef RECALC_T_MIN
	recalc_t_min = true;
#endif
	vec3 col;
	if(!cube_main(worldPos, true, recalc_t_min, col, gl_FragDepth))
		discard;
	fs_out_col = vec4(col, 1);
}