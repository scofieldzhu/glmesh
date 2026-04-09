/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: shader_program.h 
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

#ifndef __shader_program_h__
#define __shader_program_h__

#include "glmesh/core/glm_base_type.h"
#include "glmesh/core/glm_export.h"
#include "glmesh/core/instantiator.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_API ShaderProgram : public Instantiator<ShaderProgram>
{
public:
    uint32_t addShaderSource(const char* source, uint32_t shader_type);
    uint32_t addShaderFile(const char* filename, uint32_t shader_type);  
    bool link();   
    void use();
    int32_t getUniformLocation(const char* name)const;
    void setUniformMatrix4fv(int32_t location, const glm::mat4& mat)const;
    void setUniformMatrix4fv(const char* name, const glm::mat4& mat)const;
    void setUniformVec4(const char* name, const glm::vec4& v)const;
    void setUniformVec3(const char* name, const glm::vec3& v)const;
    void setUniformInt(const char* name, int v)const;
    uint32_t id()const{ return id_; }
    ShaderProgram();
    ~ShaderProgram();

private:    
    void releaseShaders();
    uint32_t id_ = 0;
    std::vector<uint32_t> shaders_;
};

GLMESH_NAMESPACE_END

#endif