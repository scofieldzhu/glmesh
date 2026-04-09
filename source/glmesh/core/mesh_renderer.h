/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_poly_data_renderer.h 
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

#ifndef __mesh_poly_data_renderer_h__
#define __mesh_poly_data_renderer_h__

#include "glmesh/core/glm_base_type.h"
#include "glmesh/core/glm_export.h"
#include "glmesh/core/instantiator.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_API MeshRenderer : public Instantiator<MeshRenderer>, public std::enable_shared_from_this<MeshRenderer>
{
public:
    void setBackgroudTopColor(const glm::vec3& color);
    void setBackgroudBottomColor(const glm::vec3& color);
    glmMeshPtr currentMeshCloud()const;
    void loadMeshCloud(glmMeshPtr mesh_cloud);
    bool initialize(float width, float height);
    void destroy();
    void resize(float width, float height);
    glm::vec2 renderSize()const{ return render_size_; }
    void render();
    auto meshActor(){ return mesh_actor_; }
    auto bkgActor(){ return bkg_; }
    glmCameraPtr activeCamera(){ return camera_; }
    MeshRenderer();
    ~MeshRenderer();

private:
    void configOpengl();
    void syncCameraToShader();
    glmShaderProgramPtr program_;
    glmCameraPtr camera_;
    glmSpherePtr sphere_;
    glmBkgActorPtr bkg_;
    glmMeshActorPtr mesh_actor_;
    glm::vec2 render_size_;
};

GLMESH_NAMESPACE_END

#endif