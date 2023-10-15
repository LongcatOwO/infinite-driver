#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform sampler2D uTex;
uniform vec2 uScreenSize;
uniform float uPatternAngle;
uniform float uFov;

#define PI 3.1415926538


// viewspace data (this must match the output of the fragment shader)
in VertexData {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
    float aspect_ratio = uScreenSize.y / uScreenSize.x;
    ivec2 tex_size = textureSize(uTex, 0);


    vec2 uv = f_in.textureCoord;
    // compensate for aspect ratio/spherical distortion
    uv.y /= 2;
    float c = cos(uPatternAngle), s = sin(uPatternAngle);

    uv = uv * mat2(
        c, -s,
        s,  c
    );

    // R-TR p.549
    vec2 wanted_res = (uScreenSize * 2) / tan(uFov/2.f);
    vec2 reps = wanted_res / tex_size;

    vec4 colour = texture(uTex, uv * reps);

    // output to the frambuffer
    fb_color = colour;
}