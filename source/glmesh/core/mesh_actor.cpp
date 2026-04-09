/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_poly_data_actor.cpp 
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

#include "mesh_actor.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glad/glad.h>
#include "shader_program.h"
#include "misc.h"
#include "buffer.h"
#include "vertex_array.h"
#include "vertex_array_attrib.h"
#include "shader_source.h"
#include "mesh_poly_data.h"
#include "mesh_renderer.h"
#include "camera.h"

GLMESH_NAMESPACE_BEGIN

namespace{
  std::map<glmDisplayMode, GLuint> stDisplayModeDict = 
  {
    {glmDisplayMode::kPoint, GL_POINTS},
    {glmDisplayMode::kWire, GL_LINE},
    {glmDisplayMode::kFacet, GL_FILL}
  };
}

MeshActor::MeshActor(glmShaderProgramPtr prog)
    :program_(prog)
{
}

MeshActor::~MeshActor()
{
    destroy();
}

bool MeshActor::setMeshCloud(glmMeshPtr mesh_cloud)
{
    if(mesh_cloud.get() == cur_mesh_cloud_.get()){
        spdlog::warn("The same mesh cloud has been loaded!");
        return false;
    }
    if(!mesh_cloud->valid()){
        spdlog::warn("Mesh cloud contain a empty vertex list!");
        return false;
    }
    cur_mesh_cloud_ = mesh_cloud;
    return true;
}

void MeshActor::updateMeshCloud(glmMeshPtr mesh_cloud)
{
    if(!setMeshCloud(mesh_cloud)){
        return;
    }
    if(existRenderer()){
        createSource(renderers_[0].get());
    }
}

bool MeshActor::createSource(MeshRenderer* ren)
{
    auto mesh_byte_size = cur_mesh_cloud_->calcTotalByteSize();
    auto boundingbox = cur_mesh_cloud_->calcBoundingBox();
    glm::vec3 center_point = cur_mesh_cloud_->calcCenterPoint();
    float diagonal_len = boundingbox.calcDiagonalLength();
    spdlog::info("diagonal_len:{} center_point:{}", diagonal_len, Vec3ToStr(center_point));
    matrix_ = glm::translate(glm::mat4(1.0), -center_point);
    auto ren_camera = ren->activeCamera();
    ren_camera->setModel(matrix_);
    // ren_camera->setEye({0.0f, 0.0f, diagonal_len * 1.6f});
    // ren_camera->setFarPlaneDist(ren_camera->eye()[2]);
    // ren_camera->setWinAspect(ren->renderSize()[0] / ren->renderSize()[1]);

    // 相机位置  
    float camera_distance = diagonal_len * 1.6f;  
    ren_camera->setEye({0.0f, 0.0f, camera_distance});  
    ren_camera->setFocalPoint({0,0,0});  // 如果有这个方法 
    ren_camera->setViewUp({0.0f, 1.0f, 0.0f}); 
    
    // 修正近远平面计算  
    float near_plane = camera_distance * 0.01f;  // 相机距离的1%  
    if(near_plane < 0.1f) near_plane = 0.1f;     // 确保近平面不会太小  
    
    float far_plane = camera_distance * 2.0f;    // 相机距离的2倍  
    
    ren_camera->setNearPlaneDist(near_plane);  
    ren_camera->setFarPlaneDist(far_plane);  
    
    ren_camera->setWinAspect(ren->renderSize()[0] / ren->renderSize()[1]);  
    

    spdlog::debug("Camera eye: ({}, {}, {})",   
                  ren_camera->eye()[0], ren_camera->eye()[1], ren_camera->eye()[2]);  
    spdlog::debug("Near plane: {}", ren_camera->nearPlaneDist());  
    spdlog::debug("Far plane: {}", ren_camera->farPlaneDist());  
    spdlog::debug("Model diagonal: {}", diagonal_len);  
    spdlog::debug("Model center: ({}, {}, {})",   
                  (boundingbox.max.x + boundingbox.min.x) / 2.0f,  
                  (boundingbox.max.y + boundingbox.min.y) / 2.0f,  
                  (boundingbox.max.z + boundingbox.min.z) / 2.0f);  

    if(vao_ == nullptr){
        vao_ = std::make_shared<VertexArray>();
    }
    vao_->bindCurrent();
    buffer_ = nullptr; //release old buffer
    if(buffer_ == nullptr){
        buffer_ = Buffer::New(GL_ARRAY_BUFFER);
        buffer_->createImmutableDataStore(MeshPolyData::kMaxVertexRelatedByteSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    vao_->bindBuffer(*buffer_);

    uint32_t vbs = cur_mesh_cloud_->calcByteSizeOfVertices();
    uint32_t cbs = cur_mesh_cloud_->calcByteSizeOfColors();
    uint32_t nbs = cur_mesh_cloud_->calcByteSizeOfNormals();
    int32_t current_offset = 0;
    buffer_->writeSubData(current_offset, vbs, cur_mesh_cloud_->vertices.data());
    vao_->getAttrib(0)->setPointer(3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(current_offset));
    vao_->getAttrib(0)->enable();
    current_offset += vbs;
    if(cbs){
        buffer_->writeSubData(current_offset, cbs, cur_mesh_cloud_->colors.data());
        vao_->getAttrib(1)->setPointer(4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(current_offset));
        vao_->getAttrib(1)->enable();
    }
    current_offset += cbs;
    if(nbs){
        buffer_->writeSubData(current_offset, nbs, cur_mesh_cloud_->normals.data());
        vao_->getAttrib(2)->setPointer(3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(current_offset));
        vao_->getAttrib(2)->enable();
    }
    current_offset += nbs;
    
    if(indices_buffer_ == nullptr){
        indices_buffer_ = Buffer::New(GL_ELEMENT_ARRAY_BUFFER);
        indices_buffer_->createImmutableDataStore(MeshPolyData::kMaxFacetByteSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    if(cur_mesh_cloud_->existFacetData()){        
        auto indices_data_mb = cur_mesh_cloud_->genFacetMemory();
        indices_buffer_->writeSubData(0, static_cast<uint32_t>(indices_data_mb->size()), indices_data_mb->blockData());
        vao_->bindBuffer(*indices_buffer_);
    }else{
        vao_->unbindBuffer(*indices_buffer_);
    }
    return true;
}

void MeshActor::setUserColor(const glm::vec4 &color)
{
    user_color_ = color;
}

void MeshActor::destroy()
{
    program_.reset();
    buffer_.reset();
    indices_buffer_.reset();
    vao_.reset();
}

void MeshActor::setDispalyMode(glmDisplayMode m)
{
    display_mode_ = m;
}

void MeshActor::draw(MeshRenderer* ren)
{
    if(vao_ == nullptr || renderers_.empty() || cur_mesh_cloud_ == nullptr || !cur_mesh_cloud_->valid()){
        return;
    }
    program_->use();
    vao_->bindCurrent();
    auto ren_camera = renderers_[0]->activeCamera();
    ren_camera->recalc();
    ren_camera->syncDataToShader(program_);

    program_->setUniformInt("primitive_type", 1);
    if(cur_mesh_cloud_->colors.empty()){
        program_->setUniformInt("use_vcolor", 0);
        program_->setUniformVec4("user_color", user_color_);
    }else{
        program_->setUniformInt("use_vcolor", 1);        
    }
    if(cur_mesh_cloud_->normals.empty()){
        program_->setUniformInt("use_vnormal", 0);
        program_->setUniformVec3("user_normal", user_normal_);
    }else{
        program_->setUniformInt("use_vnormal", 1);        
    } 
    auto gl_mode = stDisplayModeDict[display_mode_];
    glPolygonMode(GL_FRONT_AND_BACK, gl_mode);
    if(gl_mode == GL_POINTS){
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(cur_mesh_cloud_->vertices.size()));
    }else{            
        if(cur_mesh_cloud_->existFacetData()){
            if(cur_mesh_cloud_->isTriangulated()){
                glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(cur_mesh_cloud_->calcIndiceCount()), GL_UNSIGNED_INT, nullptr);
            }else{
                glEnable(GL_PRIMITIVE_RESTART);
                glPrimitiveRestartIndex(MeshPolyData::kPolyRestartIndex);
                glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(cur_mesh_cloud_->calcIndiceCount()), GL_UNSIGNED_INT, nullptr);
            }
        }
    }
}

GLMESH_NAMESPACE_END