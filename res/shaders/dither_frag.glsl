#version 330 core

in vec2 v_texCoord;
out vec4 fb_color;

uniform sampler2D uFramebuffer;
uniform sampler2D uDitherPattern;

void main() {
    vec4 buffer_colour = texture(uFramebuffer, v_texCoord);
    // https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
    float grey = 0.2126 * buffer_colour.r + 0.7152 * buffer_colour.g + 0.0722 * buffer_colour.b;

//    // fetch dither pixel
//    ivec2 frag_pos = ivec2(gl_FragCoord.xy);
//    frag_pos = frag_pos % textureSize(uDitherPattern, 0);
//    vec4 dither_point = texelFetch(uDitherPattern, frag_pos, 0);
    vec4 dither_point = texture(uDitherPattern, v_texCoord);

    vec4 colour = vec4(0, 0, 0, 1);

    // could do this channel-by-channel but can't be arsed right now
    if (grey > dither_point.r) {
        colour = colour = vec4(1, 1, 1, 1);
    } else {
        colour = vec4(0, 0, 0, 1);
    }

    fb_color = colour;
}