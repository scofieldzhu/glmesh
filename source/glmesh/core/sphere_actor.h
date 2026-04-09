/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: sphere_actor.h 
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

#ifndef __sphere_actor_h__
#define __sphere_actor_h__

#include "glmesh/core/actor.h"
#include "glmesh/core/instantiator.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_API SphereActor : public Actor, public Instantiator<SphereActor>
{
public:
    void draw(MeshRenderer* ren) override;    
    void setLongitudeResolution(uint32_t res);
    uint32_t longitudeResolution() const{ return longi_res_; }
    void setLatitudeResolution(uint32_t res);
    uint32_t latitudeResolution() const{ return latit_res_; }
    void setCenter(const glmPt3& center);    
    const auto& center() const{ return center_; }
    void setRadius(float radius);
    float radius() const{ return radius_; }
    void setColor(const glmClr3& color);
    const auto& color() const{ return color_; }
    void setShaderProgram(glmShaderProgramPtr shader_program);
    glmShaderProgramPtr shaderProgram() const{ return shader_program_; }
    SphereActor();
    SphereActor(const glmPt3& center, float radius);
    ~SphereActor();

private:
    bool createSource(MeshRenderer* ren) override;
    glmPt3 center_ = glmPt3(0.0f, 0.0f, 0.0f);
    float radius_ = 1.0f;
    glmClr3 color_ = glmClr3(1.0f, 1.0f, 1.0f);
    uint32_t longi_res_ = 180;
    uint32_t latit_res_ = 90;
    glmShaderProgramPtr shader_program_ = nullptr;
    glmVertexArrayPtr vao_ = nullptr;
    glmBufferPtr vbo_ = nullptr;
    glmBufferPtr ebo_ = nullptr;
    glmVertexList vertexes_;
    glmColorList colors_;
    glmIndices indices_;
};

GLMESH_NAMESPACE_END

#endif