#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uColour;
uniform float uShininess;

uniform vec3 uLightPos;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
    vec3 light_pos = (vec4(uLightPos, 1) * uViewMatrix).xyz;
    vec3 N = normalize(f_in.normal);
    vec3 V = normalize(-f_in.position);
    vec3 L = normalize(light_pos - f_in.position);
    vec3 H = normalize(L + V);

    vec3 ambient = 0.3 * uColour;
    vec3 diffuse = max(dot(N, L), 0) * uColour;
    vec3 specular = max(pow(dot(N, H), uShininess), 0) * vec3(1);

    vec3 colour = ambient + diffuse + specular;

    // output to the frambuffer
    fb_color = vec4(colour, 1);
}