#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;

uniform float uThreshold;

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);

    vec4 colour = vec4(0, 0, 0, 1);
    colour.r = buffer_colour.r > uThreshold ? 1 : 0;
    colour.g = buffer_colour.g > uThreshold ? 1 : 0;
    colour.b = buffer_colour.b > uThreshold ? 1 : 0;

    fb_color = colour;
}