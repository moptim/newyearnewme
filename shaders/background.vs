#version 420 core
out vec2 tex_coords;

uniform mat4 view;
uniform mat4 projection;

void main() {
	vec2 vertices[3] = vec2[3](
		vec2(-1.0, -1.0),
		vec2( 3.0, -1.0),
		vec2(-1.0,  3.0)
	);
	vec2 vertex_2d = vertices[gl_VertexID];
	vec2 vertex_2d_vertspace = vertex_2d * 100.0;
	vec2 vertex_2d_texspace  = vertex_2d * 2.0 + vec2(0.7065, 0.0); // keymash

	vec4 vertex = vec4(vertex_2d_vertspace.x, 0.0, vertex_2d_vertspace.y, 1.0);
	gl_Position = projection * view * vertex;
	tex_coords = vertex_2d_texspace;
}
