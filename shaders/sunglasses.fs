#version 330 core
in vec2 tex_coords;
out vec4 color;

uniform sampler2D view;
uniform sampler2D sunglasses;
uniform float fadeout;

vec3 hue_distortion = vec3(0.1f, 0.5f, 1.0f);

void main()
{
	vec3 scene_color = vec3(texture(view, tex_coords)) * fadeout;

	vec3 scene_sunglass_color = vec3(scene_color.b, scene_color.g, scene_color.r);

	float sunglass_blueness = clamp(scene_sunglass_color.b - scene_sunglass_color.g - scene_sunglass_color.r, 0.0, 0.8);
	scene_sunglass_color.r += sunglass_blueness * 0.3;
	scene_sunglass_color.g += sunglass_blueness * 0.2;

	float gray = dot(scene_sunglass_color, vec3(0.299, 0.587, 0.114));
	vec3 scene_grayscale_color = vec3(gray * 0.3);

	scene_sunglass_color = mix(scene_sunglass_color, scene_grayscale_color, 0.8 - sunglass_blueness);

	vec4 sg_color = texture(sunglasses, tex_coords);
	if (sg_color.r > 0.5)
		color = vec4(scene_sunglass_color, 1.0);
	else
		color = vec4(scene_color, 1.0);
}
