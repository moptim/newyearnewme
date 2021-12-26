#version 420 core

in float is_roof;
in vec3 tc_is_roof;
out vec4 color;

uniform vec3 hue;
uniform sampler2D tex;

void main()
{
	vec2 tc = tc_is_roof.xy;
	float is_roof = tc_is_roof.z;

	vec4 wall_color = vec4(hue * texture(tex, tc).r, 1.0);

	vec3 roof_hue = vec3(0.8, 0.3, 0.35);
	vec4 roof_color = vec4(roof_hue * texture(tex, tc).r, 1.0);

	color = mix(wall_color, roof_color, is_roof);
}
