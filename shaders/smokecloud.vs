#version 420 core
out vec4 tc_tcnoscale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float tex_offset;
uniform float tex_blk_inv_count;
uniform float rad_factor;

void main() {
	vec4 vertices_tc[3] = vec4[3](
		vec4(-2.0, -2.0, 0.0, 0.0),
		vec4( 6.0, -2.0, 4.0, 0.0),
		vec4(-2.0,  6.0, 0.0, 4.0)
	);
	vec4 vert_tc = vertices_tc[gl_VertexID];
	vec2 vert2d = vert_tc.xy * (rad_factor * 3.0);
	vec2 texcrd = vert_tc.zw;

	vec4 vertex = vec4(vert2d.x, 0.0, vert2d.y, 1.0);
	gl_Position = projection * view * model * vertex;

	float tc_x_incr = tex_offset * tex_blk_inv_count;

	tc_tcnoscale = texcrd.xyxy;
	tc_tcnoscale.x += tc_x_incr;
	tc_tcnoscale.x *= tex_blk_inv_count;
}
