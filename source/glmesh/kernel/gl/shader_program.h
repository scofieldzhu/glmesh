/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: shader_program.h
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
#ifndef __shader_program_h__
#define __shader_program_h__

#include "glmesh/kernel/glmesh_kernel_export.h"
#include "glmesh/kernel/glmesh_kernel_typedef.h"
#include <unordered_map>

GLMESH_NAMESPACE_BEGIN

class GLMESH_KERNEL_API ShaderProgram
{
public:
    static void UnuseAny();
    void createFromSource(std::string_view vertex_shader_source, std::string_view frag_shader_source);
    void createFromFiles(const std::string& vertex_shader_file_path, const std::string& frag_shader_file_path);
    void use() const noexcept;
    void unuse() const noexcept;
    uint32_t id() const noexcept { return program_id_; }
    void destroy() noexcept;
    int32_t uniformLocation(std::string_view name) const;
    void setInt(std::string_view name, int value) const;
    void setFloat(std::string_view name, float value) const;
    void setVec3(std::string_view name, const glm::vec3& value) const;
    void setMat4(std::string_view name, const glm::mat4& value) const;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram() = default;
    ShaderProgram(const ShaderProgram&) = delete;    
    ShaderProgram(std::string_view vertex_shader_source, std::string_view frag_shader_source);
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;
    ~ShaderProgram();

private:
    uint32_t program_id_ = 0;
    mutable std::unordered_map<std::string, int32_t> uniform_cache_;
};

GLMESH_NAMESPACE_END

#endif