#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

// framebuffer output
out vec4 fb_color;

void main() {
    fb_color = gl_FragCoord;
}