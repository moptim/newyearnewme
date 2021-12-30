#version 420 core

// in vec2 tex_coords;
in vec4 tc_tcnoscale;
out vec4 color;

uniform vec3 hue;
uniform float rad_factor;
uniform sampler2D tex;

float inv_dist(vec2 p0, vec2 p1)
{
	vec2 delta = p0 - p1;
	float dist_sqrd = dot(delta, delta);
	if (dist_sqrd == 0.0)
		return 1337.0;
	else
		return inversesqrt(dist_sqrd);
}

void main()
{
	vec2 tex_coords = tc_tcnoscale.xy;
	vec2 tex_coords_noscale = tc_tcnoscale.zw;
	float alpha = texture(tex, tex_coords).r;
	float inv_radius = rad_factor * rad_factor * inv_dist(tex_coords_noscale, vec2(0.5, 0.5));
	if (inv_radius < 0.2)
		discard;

	float size_dimming = clamp(rad_factor * 3.0, 0.0, 1.0);
	size_dimming *= size_dimming;
	float rad_fact = smoothstep(0.2, 0.9, inv_radius);
	alpha *= rad_fact * size_dimming;

	color = vec4(hue, alpha);
}
