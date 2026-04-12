#version 330 core

in vec3 vWorldNormal;
in vec3 vVertexColor;

uniform vec3 uBaseColor;
uniform vec3 uLightDir;
uniform float uAmbient;
uniform float uDiffuse;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(vWorldNormal);
    vec3 L = normalize(-uLightDir);

    float lambert = max(dot(N, L), 0.0);

    vec3 albedo = vVertexColor * uBaseColor;
    vec3 color = albedo * (uAmbient + uDiffuse * lambert);

    FragColor = vec4(color, 1.0);
}