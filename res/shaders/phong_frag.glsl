#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

uniform vec3 uColour;
uniform float uShininess;

uniform vec3 uLightDir;
uniform vec3 uCameraPos;

uniform sampler2D uShadowTex;
uniform mat4 uShadowMatrix;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
    vec3 N = normalize(f_in.normal);
    vec3 V = normalize(uCameraPos - f_in.position);
    vec3 L = normalize(-uLightDir);
    vec3 H = normalize(L + V);

    vec3 ambient = 0.3 * uColour;
    vec3 diffuse = max(dot(N, L), 0) * uColour;
    vec3 specular = vec3(max(pow(dot(N, H), uShininess), 0));

    // attenuate channels to avoid overblown highlights
    vec3 colour = ambient + diffuse * 0.25 + specular * 0.5;

    // shadow mapping
    vec4 shadow_coord = uShadowMatrix * vec4(f_in.position, 1);
    shadow_coord = shadow_coord * 0.5 + 0.5;
    float shadow_z = texture(uShadowTex, shadow_coord.xy).x;
    float our_z = shadow_coord.z;
    // from LearnOpenGL
    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
    if (shadow_z < our_z - bias) {
        colour *= 0.5;
    }

    // output to the frambuffer
    fb_color = vec4(colour, 1);
}