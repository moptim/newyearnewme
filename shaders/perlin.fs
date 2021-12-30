#version 420 core

// Take care that hei is a power of two

out vec4 color;

uniform float initial_value;
uniform highp uvec4 hei_dim_key_lvls;
uniform vec2 ampl_params; // Initial amplitude | Attenuation per octave

in vec2 tex_coords_f;

// Uses {key, key, key, key} as the 128-bit key, we're not encrypting really
// but just doing uniform noise
void tea(inout uvec2 v, uint key)
{
	const uint delta = 0x9E3779B9;
	uint k0 = key & uint(0xffff);
	uint k1 = key >> 16;
	uint sum = 0;
	uint i;

	for (i = 0; i < 32; i++) {
		sum += delta;
		v.x += ((v.y << 4) + k0) ^ (v.y + sum) ^ ((v.y >> 5) + k1);
		v.y += ((v.x << 4) + k1) ^ (v.x + sum) ^ ((v.x >> 5) + k0);
	}
}

float uvec2_to_f(uvec2 v)
{
	return float(v.x & uint(0xffff)) / float(0xffff);
}

float bilerp(float a, float b, float c, float d, float s, float t)
{
	float x = mix(a, b, t);
	float y = mix(c, d, t);
	return mix(x, y, s);
}

void main()
{
	uvec2 tex_coords = uvec2(round(tex_coords_f));

	uint hei = hei_dim_key_lvls.x;
	uint dim = hei_dim_key_lvls.y;
	uint key = hei_dim_key_lvls.z;
	uint lvls = hei_dim_key_lvls.w;

	uint max_wid = hei * dim;
	uint max_hei = hei;

	uint mwmask = max_wid - 1;
	uint mhmask = max_hei - 1;

	uint scale = hei;
	float inv_scale = 1.0 / float(scale);
	float amplitude = ampl_params.x;
	float ampl_prog = ampl_params.y;

	float value = initial_value;
	for (uint lvl = 0; lvl < lvls; lvl++) {
		uint scale_mask = ~(scale - 1);

		uvec2 pos00 = uvec2(tex_coords.x & scale_mask, tex_coords.y & scale_mask);
		uvec2 pos11 = uvec2((pos00.x + scale) & mwmask, (pos00.y + scale) & mhmask);
		uvec2 pos01 = uvec2(pos00.x, pos11.y);
		uvec2 pos10 = uvec2(pos11.x, pos00.y);

		vec2 rel_pos = (tex_coords_f - vec2(pos00)) * inv_scale;

		tea(pos00, key);
		tea(pos01, key);
		tea(pos10, key);
		tea(pos11, key);

		float v00 = uvec2_to_f(pos00);
		float v01 = uvec2_to_f(pos01);
		float v10 = uvec2_to_f(pos10);
		float v11 = uvec2_to_f(pos11);

		float curr = bilerp(v00, v01, v10, v11, rel_pos.x, rel_pos.y);
		value += curr * amplitude;

		scale >>= 1;
		inv_scale *= 2.0;
		amplitude *= ampl_prog;

	}
	color = vec4(value, 0.0, 0.0, 1.0);
}
