#version 330 core
in vec2 fbo_coords;
// out vec4 color;
out float color;

uniform vec4 ab;
uniform float inv_thickness;
// uniform sampler1D flag;
uniform sampler2D old_buf;

float line_strength()
{
	vec2 a = ab.xy;
	vec2 b = ab.zw;
	vec2 ba = b - a;
	vec2 pa = fbo_coords - a;

	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	float dist = length(pa - h * ba);

	return 1.0 - dist * inv_thickness;
}

void main()
{
    vec2 a = ab.xy;
    vec2 b = ab.zw;

    float alpha = line_strength();
    float sampled = texture(old_buf, fbo_coords).r;
    color = clamp(max(alpha, sampled), 0.0, 1.0);

    color = 1.0;

    // vec3 sampled = texture(flag, fbo_coords.x).rgb;
    // color = vec4(sampled, alpha);
    // color = vec4(sampled, 1.0);
    // color = vec4(1.0, 1.0, 1.0, 1.0);
}
