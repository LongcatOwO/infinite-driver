#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;
uniform ivec2 uScreenSize;
uniform float uWidth;

vec2 neighbors[9] = vec2[](
    vec2(-1, 1), // top-left
    vec2(0.0f, 1), // top-center
    vec2(1, 1), // top-right
    vec2(-1, 0.0f), // center-left
    vec2(0.0f, 0.0f), // center-center
    vec2(1, 0.0f), // center-right
    vec2(-1, -1), // bottom-left
    vec2(0.0f, -1), // bottom-center
    vec2(1, -1)// bottom-right
);

// https://en.wikipedia.org/wiki/Sobel_operator#Alternative_operators
const int sobelX[9] = int[](
    +3,     0,      -3,
    +10,    0,      -10,
    +3,     0,      -3
);

const int sobelY[9] = int[](
    +3,     +10,    +3,
    0,       0,      0,
    -3,     -10,    -3
);

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);
    float depth = 1.0 - buffer_colour.r;

    vec2 step = 1.f / vec2(uScreenSize);

    float sumX = 0, sumY = 0;
    for (int i = 0; i < 9; i++) {
        float pt = texture(uFramebuffer, v_texCoord + (neighbors[i] * step)).r;
        sumX += pt * float(sobelX[i]);
        sumY += pt * float(sobelY[i]);
    }

    float edge = sqrt((sumX * sumX) + (sumY * sumY));

    if (edge > (depth*16) / uWidth) {
        fb_color = vec4(1);
    } else {
        fb_color = vec4(0, 0, 0, 0);
    }

}