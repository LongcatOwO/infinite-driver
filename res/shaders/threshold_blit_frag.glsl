#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);

    buffer_colour = buffer_colour.r > 0.45 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1);

    fb_color = buffer_colour;
}