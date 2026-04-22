/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: shader_program.cpp
 *  Copyright (c) 2024-2026 scofieldzhu
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
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

GLMESH_NAMESPACE_BEGIN

namespace{
    std::string GetShaderLogInfoString(GLuint shader_id)
    {
        GLint log_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        std::string log;
        if(log_length > 0){
            log.resize(static_cast<std::size_t>(log_length));
            glGetShaderInfoLog(shader_id, log_length, nullptr, log.data());
        }
        return log;
    }

    GLuint CompileShader(GLenum type, std::string_view source)
    {
        const GLuint shader_id = ::glCreateShader(type);
        const auto source_data = reinterpret_cast<const GLchar*>(source.data());
        const auto data_length = static_cast<GLint>(source.size());
        glShaderSource(shader_id, 1, &source_data, &data_length);
        glCompileShader(shader_id);
        GLint success = GL_FALSE;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if(success == GL_TRUE){
            return shader_id;
        }
        std::string log = GetShaderLogInfoString(shader_id);
        glDeleteShader(shader_id);
        const char* shader_type_str = (type == GL_VERTEX_SHADER ? "vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : "unknown"));
        throw std::runtime_error(std::string("failed to compile ") + shader_type_str + " shader:\n" + log);
    }

    std::string GetProgramLogInfoString(GLuint program_id)
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
        std::string log;
        if(log_length > 0){
            log.resize(static_cast<std::size_t>(log_length));
            glGetProgramInfoLog(program_id, log_length, nullptr, log.data());
        }
        return log;
    }

    GLuint LinkProgram(GLuint vertex_shader_id, GLuint frag_shader_id)
    {
        const GLuint program_id = glCreateProgram();
        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, frag_shader_id);
        glLinkProgram(program_id);
        GLint success = GL_FALSE;
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if(success == GL_TRUE){
            return program_id;
        }
        std::string log = GetProgramLogInfoString(program_id);
        glDeleteProgram(program_id);
        throw std::runtime_error("failed to link shader program:\n" + log);
    }

    std::string ReadTextFile(const std::string& path)
    {
        std::ifstream ifs(path, std::ios::binary);
        if(!ifs){
            throw std::runtime_error("failed to open shader file: " + path);
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        return oss.str();
    }
}

ShaderProgram::ShaderProgram(std::string_view vertex_shader_source, std::string_view frag_shader_source)
{
    createFromSource(vertex_shader_source, frag_shader_source);
}

ShaderProgram::~ShaderProgram()
{
    destroy();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    :program_id_(std::exchange(other.program_id_, 0)),
    uniform_cache_(std::move(other.uniform_cache_))
{
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if(this != &other){
        destroy();
        program_id_ = std::exchange(other.program_id_, 0);
        uniform_cache_ = std::move(other.uniform_cache_);
    }
    return *this;
}

void ShaderProgram::createFromSource(std::string_view vertex_shader_source, std::string_view frag_shader_source)
{
    GLuint vertex_shader_id = 0;
    GLuint frag_shader_id = 0;
    GLuint program_id = 0;
    try{
        vertex_shader_id = CompileShader(GL_VERTEX_SHADER, vertex_shader_source);
        frag_shader_id = CompileShader(GL_FRAGMENT_SHADER, frag_shader_source);
        program_id = LinkProgram(vertex_shader_id, frag_shader_id);
    }catch(...){
        if(vertex_shader_id != 0){
            glDeleteShader(vertex_shader_id);
        }
        if(frag_shader_id != 0){
            glDeleteShader(frag_shader_id);
        }
        if(program_id != 0){
            glDeleteProgram(program_id);
        }
        throw;
    }
    glDeleteShader(vertex_shader_id);
    glDeleteShader(frag_shader_id);
    destroy();
    program_id_ = program_id;
    uniform_cache_.clear();
}

void ShaderProgram::createFromFiles(const std::string& vertex_shader_file_path, const std::string& frag_shader_file_path)
{
    const std::string vertex_shader_source = ReadTextFile(vertex_shader_file_path);
    const std::string frag_shader_source = ReadTextFile(frag_shader_file_path);
    createFromSource(vertex_shader_source, frag_shader_source);
}

void ShaderProgram::use() const noexcept
{
    glUseProgram(program_id_);
}

void ShaderProgram::destroy() noexcept
{
    if(program_id_ != 0){
        glDeleteProgram(program_id_);
        program_id_ = 0;
    }
    uniform_cache_.clear();
}

GLint ShaderProgram::uniformLocation(std::string_view name) const
{
    const std::string key(name);
    const auto it = uniform_cache_.find(key);
    if(it != uniform_cache_.end()){
        return it->second;
    }
    const GLint location = glGetUniformLocation(program_id_, key.c_str());
    uniform_cache_.emplace(key, location);
    return location;
}

void ShaderProgram::setInt(std::string_view name, int value) const
{
    const GLint loc = uniformLocation(name);
    if(loc != -1){
        glUniform1i(loc, value);
    }
}

void ShaderProgram::setFloat(std::string_view name, float value) const
{
    const GLint loc = uniformLocation(name);
    if(loc != -1){
        glUniform1f(loc, value);
    }
}

void ShaderProgram::setVec3(std::string_view name, const glm::vec3& value) const
{
    const GLint loc = uniformLocation(name);
    if(loc != -1){
        glUniform3fv(loc, 1, glm::value_ptr(value));
    }
}

void ShaderProgram::setMat4(std::string_view name, const glm::mat4& value) const
{
    const GLint loc = uniformLocation(name);
    if(loc != -1){
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
}

GLMESH_NAMESPACE_END