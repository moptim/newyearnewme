#version 420 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in float is_roof;

out vec3 tc_is_roof;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float tex_offset;
uniform float tex_blk_inv_count;

void main()
{
	gl_Position = projection * view * model * vec4(vertex, 1.0);
	tc_is_roof = vec3(tex_coords, is_roof);
	tc_is_roof.x += tex_offset;
	tc_is_roof.x *= tex_blk_inv_count;
}
