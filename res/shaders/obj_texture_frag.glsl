#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform sampler2D uTex;
uniform vec2 uScreenSize;

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

//    float reps = (uScreenSize.x / tex_size.x);
//    vec2 scale = vec2(reps * aspect_ratio, reps/ aspect_ratio);

    // assumes fov == 1rad, R-TR p.549
    vec2 wanted_res = uScreenSize / 0.54630248984;
    vec2 reps = wanted_res / tex_size;
    vec2 scale = reps * 2;

    vec4 colour = texture(uTex, f_in.textureCoord * scale);


    // output to the frambuffer
    fb_color = colour;
}