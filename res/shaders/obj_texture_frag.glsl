#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform sampler2D uTex;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
    vec4 colour = texture(uTex, f_in.textureCoord);

    // output to the frambuffer
    fb_color = colour;
}