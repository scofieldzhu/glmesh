/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: bkg_actor.cpp 
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

#include "bkg_actor.h"
#include <glad/glad.h>
#include "shader_program.h"
#include "shader_source.h"
#include "buffer.h"
#include "vertex_array.h"

GLMESH_NAMESPACE_BEGIN

glmBkgActor::glmBkgActor()
{
}

glmBkgActor::~glmBkgActor()
{
}

bool glmBkgActor::createSource(MeshRenderer* ren)
{
    program_ = ShaderProgram::New();
    if(!program_->addShaderSource(ShaderSource::kBkgVertexShaderSource, GL_VERTEX_SHADER))
        return false;
    if(!program_->addShaderSource(ShaderSource::kBkgFragmentShaderSource, GL_FRAGMENT_SHADER))
        return false;
    if(!program_->link())
        return false;
    program_->use();
    float rt_vertices[] = 
    {
        -1.0f, -1.0f, 0.0f,  bottom_color_[0], bottom_color_[1], bottom_color_[2], 
        1.0f, -1.0f, 0.0f,  bottom_color_[0], bottom_color_[1], bottom_color_[2],
        -1.0f,  1.0f, 0.0f,  top_color_[0], top_color_[1], top_color_[2],
        1.0f,  1.0f, 0.0f,  top_color_[0], top_color_[1], top_color_[2]
    };
    vertex_buffer_ = Buffer::New(GL_ARRAY_BUFFER);
    vertex_buffer_->createImmutableDataStore(sizeof(rt_vertices), rt_vertices, 0);
    vao_ = VertexArray::New();
    vao_->bindCurrent();
    vao_->bindBuffer(*vertex_buffer_);
    vao_->getAttrib(0)->setPointer(3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(0));
    vao_->getAttrib(0)->enable();
    vao_->getAttrib(1)->setPointer(3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
    vao_->getAttrib(1)->enable();
    return true;
}

void glmBkgActor::draw(MeshRenderer* ren)
{
    if(program_){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        program_->use();
        vao_->bindCurrent();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  
    }
}

void glmBkgActor::setTopColor(const glm::vec3 &clr)
{
    top_color_ = clr;
}

void glmBkgActor::setBottomColor(const glm::vec3 &clr)
{
    bottom_color_ = clr;
}



GLMESH_NAMESPACE_END