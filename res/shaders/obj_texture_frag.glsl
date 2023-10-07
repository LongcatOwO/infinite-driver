#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform sampler2D uTex;
uniform vec2 uScreenSize;
uniform float uPatternAngle;

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
    float aspect_ratio = uScreenSize.x / uScreenSize.y;
    ivec2 tex_size = textureSize(uTex, 0);


    vec2 uv = f_in.textureCoord;
    float c = cos(uPatternAngle), s = sin(uPatternAngle);
    uv = uv * mat2(
        c, -s,
        s,  c
    );

    // assumes fov == 1rad, R-TR p.549
    vec2 wanted_res = (uScreenSize*1) / tan(1.f/2.f);
    // this is fov==90deg
//    vec2 wanted_res = uScreenSize / 1;
    vec2 reps = wanted_res / tex_size;
    vec2 scale = reps;

    vec4 colour = texture(uTex, uv * scale);

    // output to the frambuffer
    fb_color = colour;
}