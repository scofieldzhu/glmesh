/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_poly_data_actor.h 
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

#ifndef __mesh_actor_h__
#define __mesh_actor_h__

#include "glmesh/core/actor.h"
#include "glmesh/core/instantiator.h"
#include "glmesh/core/glm_export.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_API MeshActor : public Actor, public Instantiator<MeshActor>
{
public:
    auto currentMeshCloud()const{ return cur_mesh_cloud_; }
    void updateMeshCloud(glmMeshPtr mesh_cloud);
    void draw(MeshRenderer* ren) override;
    void setUserColor(const glm::vec4& color);
    void destroy();
    void setDispalyMode(glmDisplayMode m);
    auto displayMode()const{ return display_mode_; }
    auto program()const{ return program_; }
    MeshActor(glmShaderProgramPtr prog);
    ~MeshActor();

private:
    bool setMeshCloud(glmMeshPtr mesh_cloud);
    bool createSource(MeshRenderer* ren) override;
    float renderer_width_ = 0.0f, renderer_height_ = 0.0f;
    glm::vec4 user_color_ = glm::vec4(1.0, 1.0, 0.0, 1.0);
    glm::vec3 user_normal_ = glm::vec3(0.0f, 0.0f, 1.0f);
    glmMeshPtr cur_mesh_cloud_;
    glmBufferPtr buffer_;
    glmBufferPtr indices_buffer_;
    glmVertexArrayPtr vao_;
    glmShaderProgramPtr program_;
    glmDisplayMode display_mode_ = glmDisplayMode::kPoint;
};

GLMESH_NAMESPACE_END

#endif

