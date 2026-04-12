/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_poly_data_renderer.cpp 
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

#include "mesh_renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <spdlog/spdlog.h>
#include "glad/glad.h"
#include "mesh_poly_data.h"
#include "shader_program.h"
#include "sphere_actor.h"
#include "bkg_actor.h"
#include "mesh_actor.h"
#include "camera.h"
#include "shader_source.h"

GLMESH_NAMESPACE_BEGIN

MeshRenderer::MeshRenderer()
    :bkg_(glmBkgActor::New()),
    camera_(Camera::New())
{
}

MeshRenderer::~MeshRenderer()
{
    destroy();
}

void MeshRenderer::setBackgroudBottomColor(const glm::vec3& color)
{
    bkg_->setBottomColor(color);
}

glmMeshPtr MeshRenderer::currentMeshCloud() const
{
    return mesh_actor_->currentMeshCloud();
}

void MeshRenderer::setBackgroudTopColor(const glm::vec3& color)
{
    bkg_->setTopColor(color);
}

void MeshRenderer::loadMeshCloud(glmMeshPtr mesh_cloud)
{
    if(mesh_actor_ == nullptr){
        mesh_actor_ = MeshActor::New(program_);
    }
    mesh_actor_->updateMeshCloud(mesh_cloud);
    if(!mesh_actor_->existRenderer()){
        mesh_actor_->addToRenderer(shared_from_this());
    }    
}

bool MeshRenderer::initialize(float width, float height)
{
    if(!gladLoadGL()){
        spdlog::error("gladLoadGL failed!");
        return false;
    }

    program_ = ShaderProgram::New();
    if(!program_->addShaderSource(ShaderSource::kVertexShaderSource, GL_VERTEX_SHADER))
        return false;
    if(!program_->addShaderSource(ShaderSource::kFragmentShaderSource, GL_FRAGMENT_SHADER))
        return false;
    if(!program_->link())
        return false;
    program_->use();

    bkg_->addToRenderer(this->shared_from_this());

    // sphere_ = glmSphereActor::New();
    // sphere_->setShaderProgram(program_);
    // sphere_->createSource();
    return true;
}

void MeshRenderer::destroy()
{
    bkg_.reset();
    mesh_actor_.reset();
}

void MeshRenderer::configOpengl()
{
    

}

void MeshRenderer::render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    //glEnable(GL_DEPTH_TEST);  
    glDepthFunc(GL_LESS);
    //glDisable(GL_CULL_FACE);
    bkg_->draw(this);
    if(sphere_){
        program_->use();
        program_->setUniformInt("primitive_type", 0);
        program_->setUniformInt("use_vcolor", 1);
        //program_->setUniformVec4("user_color", user_color_);
        sphere_->draw(this);
    }
    if(mesh_actor_)
        mesh_actor_->draw(this);
}

void MeshRenderer::resize(float width, float height)
{
    //spdlog::info("glmMeshRenderer resize!");
    glViewport(0, 0, (int)width, (int)height);
    render_size_ = {width, height};
}

GLMESH_NAMESPACE_END