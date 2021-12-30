#version 420 core

uniform vec3 hue;

out vec4 color;

void main()
{
	// color = vec4(1.0, 1.0, 0.1 * hue.x, 1.0);
	color = vec4(hue, 1.0);
}
