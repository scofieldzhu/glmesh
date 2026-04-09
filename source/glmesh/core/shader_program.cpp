/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: shader_program.cpp 
 *  Copyright (c) 2024-2024 scofieldzhu
 *  
 *  MIT License
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "shader_program.h"
#include "glad/glad.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

GLMESH_NAMESPACE_BEGIN

namespace{
    std::string ReadShaderFile(const char* filename)
    {
        std::string source;
        std::FILE* fp = std::fopen(filename, "rb");
        if(fp == nullptr)
            return source;
        std::fseek(fp, 0, SEEK_END);
        size_t size = std::ftell(fp);
        std::fseek(fp, 0, SEEK_SET);
        source.resize(size);
        std::fread(source.data(), 1, size, fp);
        std::fclose(fp);
        return source;
    }
}

ShaderProgram::ShaderProgram()
{
    id_ = glCreateProgram();
    assert(id_);
}

ShaderProgram::~ShaderProgram()
{
    releaseShaders();
    glDeleteProgram(id_);
}

void ShaderProgram::releaseShaders()
{
    for(auto sid : shaders_)
        glDeleteShader(sid);
    shaders_.clear();
}

uint32_t ShaderProgram::addShaderSource(const char* source, uint32_t shader_type)
{
    if(source == nullptr || shader_type == GL_NONE){
        spdlog::error("Null value of parameter 'source' or 'shader_type'!");
        return 0;
    }
    GLuint shader_id = glCreateShader(shader_type);
    const GLchar* source_data = source;
    const GLint length = (int)strlen(source);
    glShaderSource(shader_id, 1, &source_data, &length);
    glCompileShader(shader_id);
    GLint compiled;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled );
    if(!compiled){
        GLsizei len;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &len);
        std::string log_msg(len + 1, 0);
        glGetShaderInfoLog(shader_id, len, &len, log_msg.data());
        spdlog::error("Compile shader source error:{}", log_msg);
        return 0;
    }
    glAttachShader(id_, shader_id);
    shaders_.push_back(shader_id);
    return shader_id;
}

uint32_t ShaderProgram::addShaderFile(const char* filename, uint32_t shader_type)
{
    if(shader_type == GL_NONE || filename == nullptr){
        spdlog::error("Null value of parameter 'filename' or 'shader_type'!");
        return 0;
    }
    auto source = ReadShaderFile(filename);
    if(source.empty()){
        spdlog::error("Unable to read content from file:{}", std::string(filename));
        return 0;
    }
    return addShaderSource(source.c_str(), shader_type);
}

bool ShaderProgram::link()
{
    glLinkProgram(id_);
    GLint linked;
    glGetProgramiv(id_, GL_LINK_STATUS, &linked);
    if(!linked){
        GLsizei len;
        glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &len);
        std::string log_msg(len + 1, 0);
        glGetProgramInfoLog(id_, len, &len, log_msg.data());
        spdlog::error("Shader linking failed:{}", log_msg);
        releaseShaders();
    }
    return (bool)linked;
}

void ShaderProgram::use()
{
    glUseProgram(id_);
}

int32_t ShaderProgram::getUniformLocation(const char* name) const
{
    return glGetUniformLocation(id_, name);
}

void ShaderProgram::setUniformMatrix4fv(int32_t location, const glm::mat4& mat) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setUniformMatrix4fv(const char* name, const glm::mat4 &mat) const
{
    setUniformMatrix4fv(glGetUniformLocation(id_, name), mat);
}

void ShaderProgram::setUniformVec4(const char* name, const glm::vec4& v) const
{
    glUniform4f(glGetUniformLocation(id_, name), v[0], v[1], v[2], v[3]);
}

void ShaderProgram::setUniformVec3(const char *name, const glm::vec3& v) const
{
    glUniform3f(glGetUniformLocation(id_, name), v[0], v[1], v[2]);
}

void ShaderProgram::setUniformInt(const char* name, int v) const
{
    glUniform1i(glGetUniformLocation(id_, name), v);
}

GLMESH_NAMESPACE_END
