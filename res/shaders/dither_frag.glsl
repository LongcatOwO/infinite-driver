#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform bool uDitherColour;

uniform sampler2D uFramebuffer;
uniform sampler2D uDitherPattern;

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);
    // https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
    float grey = 0.2126 * buffer_colour.r + 0.7152 * buffer_colour.g + 0.0722 * buffer_colour.b;
    vec3 source = uDitherColour ? buffer_colour.rgb : vec3(grey);

//    // fetch dither pixel
//    ivec2 frag_pos = ivec2(gl_FragCoord.xy);
//    frag_pos = frag_pos % textureSize(uDitherPattern, 0);
//    vec4 dither_point = texelFetch(uDitherPattern, frag_pos, 0);
    vec4 dither_point = texture(uDitherPattern, v_texCoord);

    vec4 colour = vec4(0, 0, 0, 1);

    colour.r = source.r > dither_point.r ? 1 : 0;
    colour.g = source.g > dither_point.g ? 1 : 0;
    colour.b = source.b > dither_point.b ? 1 : 0;

    fb_color = colour;
}