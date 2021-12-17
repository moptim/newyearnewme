#version 330 core
in vec2 tex_coords;
out vec4 color;

uniform sampler2D buf;
uniform sampler1D rainbow;

void main()
{
    // color = vec4(1.0, 0.0, 1.0, texture(buf, tex_coords).r);
    color = vec4(texture(rainbow, tex_coords.x).rgb, texture(buf, tex_coords).r);
}
