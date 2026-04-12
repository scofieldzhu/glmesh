#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldNormal;
out vec3 vVertexColor;

void main()
{
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
    vWorldNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vVertexColor = aColor;
}