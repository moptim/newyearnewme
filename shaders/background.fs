#version 420 core

in vec2 tex_coords;
out vec4 color;

uniform sampler2D tex;

vec3 grass_hue = vec3(0.0, 1.0, 0.0);
vec3 asphalt_hue = vec3(0.2, 0.2, 0.2);

void main()
{
	vec3 hue;
	float alpha = texture(tex, tex_coords).r;
	float border = smoothstep(0.09, 0.095, abs(tex_coords.y));

	hue = mix(asphalt_hue, grass_hue, border);
	color = vec4(hue * alpha, 1.0);
}
