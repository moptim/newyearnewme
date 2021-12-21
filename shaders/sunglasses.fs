#version 330 core
in vec2 tex_coords;
out vec4 color;

uniform sampler2D view;
uniform sampler2D sunglasses;

void main()
{
	// color = vec4(1.0, tex_coords, 1.0);

	// color = texture(sunglasses, tex_coords);
	color = texture(view, tex_coords);
}
