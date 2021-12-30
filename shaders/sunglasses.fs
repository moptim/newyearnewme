#version 330 core
in vec2 tex_coords;
out vec4 color;

uniform sampler2D view;
uniform sampler2D sunglasses;

vec3 hue_distortion = vec3(0.1f, 0.5f, 1.0f);

void main()
{
	// color = vec4(1.0, tex_coords, 1.0);

	// color = texture(sunglasses, tex_coords);
	vec3 scene_color = vec3(texture(view, tex_coords));

	// color = vec4(scene_color.b, scene_color.g, scene_color.r, 1.0);
	color = vec4(scene_color, 1.0);

	// vec4 sg_color = texture(sunglasses, tex_coords);
	// if (sg_color.r > 0.5)
		// color = vec4(scene_color.b, scene_color.g, scene_color.r, 1.0);
	// else
		// color = vec4(scene_color, 1.0);

	// color = vec4(scene_color + hue_distortion - vec3(0.5, 0.5, 0.5), 1.0);
}
