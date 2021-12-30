#version 420 core

in vec2 tex_coords;
out vec4 color;

uniform sampler2D tex;

vec3 grass_hue = vec3(0.0, 1.0, 0.0);

void main()
{
	float alpha = texture(tex, tex_coords).r;
	color = vec4(grass_hue * alpha, 1.0);
}
