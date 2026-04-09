#pragma once

namespace ShaderSource {
    static const char* kVertexShaderSource = R"(#version 400 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec4 vertex_clr;
layout(location = 2) in vec3 vertex_normal;
uniform vec4 user_color;
uniform int use_vcolor;
uniform vec3 user_normal;
uniform int use_vnormal;

const int PRIMITIVE_TYPE_SPHERE = 0;
const int PRIMITIVE_TYPE_CLOUD_MESH = 1;
uniform int primitive_type;

out vec4 out_color;

void main()
{
    if(primitive_type == PRIMITIVE_TYPE_SPHERE){
        gl_Position = projection * view * vec4(vertex_pos, 1.0);
    }else if(primitive_type == PRIMITIVE_TYPE_CLOUD_MESH){
        gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
    }
    if(use_vcolor == 1){
        out_color = vertex_clr;
    }else{
        out_color = user_color;
    }        
})";
    static const char* kBkgVertexShaderSource = R"(#version 400 core

layout (location = 0) in vec3 vertex_pos; 
layout (location = 1) in vec3 vertex_color;

out vec3 out_color; 

void main()
{
    gl_Position = vec4(vertex_pos, 1.0);
    out_color = vertex_color;
})";
    static const char* kCubeVertexShaderSource = R"(#version 400 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
})";
    static const char* kFragmentShaderSource = R"(#version 450 core

in vec4 out_color;
out vec4 frag_color;

void main()
{
    frag_color = out_color;
})";
    static const char* kBkgFragmentShaderSource = R"(#version 450 core

out vec4 frag_color;

in vec3 out_color; 

void main()
{
    frag_color = vec4(out_color, 1.0);
}
)";
    static const char* kCubeFragmentShaderSource = R"(#version 450 core

out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor * objectColor, 1.0);
})";
}
