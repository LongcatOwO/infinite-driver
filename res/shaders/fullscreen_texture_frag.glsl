#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);

    fb_color = buffer_colour;
}