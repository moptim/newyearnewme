#version 330 core
in vec2 tex_coords;
out vec4 color;

uniform sampler2D text;
uniform vec4 fgcolor;

void main()
{
    float alpha = min(1.0, fgcolor.w);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coords).r);
    color = vec4(fgcolor.xyz, alpha) * sampled;
}
