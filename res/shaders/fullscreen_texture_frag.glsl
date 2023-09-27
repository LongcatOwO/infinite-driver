#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;

void main() {
    fb_color = texture(uFramebuffer, v_texCoord);
}