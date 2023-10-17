#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;

uniform float uThreshold;
uniform bool uDitherColour;

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);
    // https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
    float grey = 0.2126 * buffer_colour.r + 0.7152 * buffer_colour.g + 0.0722 * buffer_colour.b;
    vec3 source = uDitherColour ? buffer_colour.rgb : vec3(grey);

    vec4 colour = vec4(0, 0, 0, 1);
    colour.r = source.r > uThreshold ? 1 : 0;
    colour.g = source.g > uThreshold ? 1 : 0;
    colour.b = source.b > uThreshold ? 1 : 0;

    fb_color = colour;
}