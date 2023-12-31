#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoord;

out vec2 v_texCoord;

void main() {
    v_texCoord = aTexCoord;
    gl_Position = vec4(aPosition.x, aPosition.y, 0 ,1);
}