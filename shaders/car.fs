#version 420 core

in vec2 tex_coords;
in flat vec3 normal;
out vec4 color;

uniform vec3 hue;
uniform vec3 light_dir;
uniform sampler2D tex;

void main()
{
	float brightness = dot(normalize(normal), normalize(light_dir));
	brightness = clamp(brightness, 0.2, 1.0);
	// color = vec4(hue * texture(tex, tex_coords).r * brightness, 1.0);
	color = vec4(texture(tex, tex_coords).rgb * brightness, 1.0);
	// color = vec4(normal, 1.0);
}
