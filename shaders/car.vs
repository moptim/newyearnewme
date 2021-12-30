#version 420 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal_in;
layout (location = 2) in vec2 tex_coords_in;

out flat vec3 normal;
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vertex, 1.0);
	tex_coords = tex_coords_in;
	tex_coords.y = 1.0 - tex_coords.y; // lazy
	normal = normal_in;
}
