#version 330 core
out vec2 tex_coords;

void main() {
	vec2 vertices[3] = vec2[3](
		vec2(-1.0, -1.0),
		vec2( 3.0, -1.0),
		vec2(-1.0,  3.0)
	);
        gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
        tex_coords = 0.5 * gl_Position.xy + vec2(0.5);
}
