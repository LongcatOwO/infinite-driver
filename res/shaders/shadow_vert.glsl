#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

// mesh data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

void main() {
    // transform vertex data to viewspace
    mat4 modelView = uViewMatrix * uModelMatrix;

    gl_Position = (uProjectionMatrix * modelView) * vec4(aPosition, 1);
}