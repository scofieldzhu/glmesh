/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: glm_sphere_actor.cpp 
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

#include "glm_sphere_actor.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include "glm_vertex_array.h"
#include "glm_buffer.h"
#include "glm_vertex_array_attrib.h"
#include "glm_shader_program.h"

GLMESH_NAMESPACE_BEGIN

glmSphereActor::glmSphereActor()
{
}

glmSphereActor::glmSphereActor(const glm::vec3& center, float radius)
    :center_(center), 
    radius_(radius)
{
}

glmSphereActor::~glmSphereActor()
{
}

void glmSphereActor::setColor(const glm::vec3& color)
{
    color_ = color;
}

void glmSphereActor::setShaderProgram(glmShaderProgramPtr shader_program)
{
    shader_program_ = shader_program;
}

void glmSphereActor::draw(glmMeshRenderer* ren)
{
    // if(shader_program_ == nullptr){
    //     spdlog::warn("shader program is not set.");
    //     return false;
    // }
    // shader_program_->use();
    vao_->bindCurrent();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
}

bool glmSphereActor::createSource(glmMeshRenderer* ren)
{
    vao_ = glmVertexArray::New();
    vao_->bindCurrent();

    indices_.clear();
    colors_.clear();
    vertexes_.clear();
    int theta_angle_step = 180 / latit_res_;
    int phi_angle_step = 360 / longi_res_;
    int r = 0, c = 0;
    for(int t = 0; t < 180; r++, t += theta_angle_step){
        c = 0;
        for(int p = 0; p < 360; c++, p += phi_angle_step){
            glmVertex pos;
            pos.x = radius_ * sin(glm::radians((float)t)) * cos(glm::radians((float)p));
            pos.y = radius_ * sin(glm::radians((float)t)) * sin(glm::radians((float)p));
            pos.z = radius_ * cos(glm::radians((float)t));
            vertexes_.push_back(pos);
            glmClr color = {(float)r / (float)latit_res_, 0.5f, 0.5f, 1.0f};
            colors_.push_back(color);
            if(r && c){
                int cur_idx = longi_res_ * r + c;
                int pre_idx = cur_idx - 1;
                int last_cur_idx = longi_res_ * (r - 1) + c;
                int last_pre_idx = last_cur_idx - 1;
                indices_.push_back(last_pre_idx);
                indices_.push_back(pre_idx);
                indices_.push_back(cur_idx);
                indices_.push_back(last_pre_idx);
                indices_.push_back(last_cur_idx);
                indices_.push_back(cur_idx);
            }                
        }        
    }
    vbo_ = glmBuffer::New(GL_ARRAY_BUFFER);
    vao_->bindBuffer(*vbo_);
    uint32_t vbs = vertexes_.size() * kVertexTypeSize;
    uint32_t cbs = colors_.size() * kColorTypeSize;
    vbo_->createImmutableDataStore(vbs + cbs, nullptr, GL_DYNAMIC_STORAGE_BIT);
    vbo_->writeSubData(0, vbs, vertexes_.data());
    vbo_->writeSubData(vbs, cbs, colors_.data());
    vao_->getAttrib(0)->setPointer(3, GL_FLOAT, GL_FALSE, kVertexTypeSize, BUFFER_OFFSET(0));
    vao_->getAttrib(0)->enable();
    vao_->getAttrib(1)->setPointer(4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vbs));
    vao_->getAttrib(1)->enable();

    ebo_ = glmBuffer::New(GL_ELEMENT_ARRAY_BUFFER);
    ebo_->createImmutableDataStore(indices_.size() * sizeof(glmIndex), indices_.data(), 0);
    vao_->bindBuffer(*ebo_);

    return true;
}

void glmSphereActor::setLongitudeResolution(uint32_t res)
{
    longi_res_ = std::clamp(res, (uint32_t)0, (uint32_t)360);
}

void glmSphereActor::setLatitudeResolution(uint32_t res)
{
    latit_res_ = std::clamp(res, (uint32_t)1, (uint32_t)180);
}

void glmSphereActor::setCenter(const glm::vec3 &center)
{
    center_ = center;
}

void glmSphereActor::setRadius(float radius)
{
    radius_ = radius;
}

GLMESH_NAMESPACE_END