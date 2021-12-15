#version 330 core
in vec2 tex_coords;
out vec4 color;

uniform sampler2D buf;

void main()
{
    // color = vec4(texture(buf, tex_coords).rgb, 1.0);
    vec4 color1 = vec4(0.0, 0.7, 1.0, 1.0);
    vec4 color2 = vec4(texture(buf, tex_coords).rrr, 1.0);
    color = mix(color1, color2, 0.95);
}
