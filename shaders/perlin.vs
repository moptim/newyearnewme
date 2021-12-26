#version 420 core
out vec2 tex_coords_f;

uniform uvec4 hei_dim_key_lvls;

void main() {
	vec2 vertices[3] = vec2[3](
		vec2(-1.0, -1.0),
		vec2( 3.0, -1.0),
		vec2(-1.0,  3.0)
	);
	uint hei = hei_dim_key_lvls.x;
	uint dim = hei_dim_key_lvls.y;

        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);

	tex_coords_f = (0.5 * gl_Position.xy + vec2(0.5)) * float(hei);
	tex_coords_f.x *= float(dim);
}
