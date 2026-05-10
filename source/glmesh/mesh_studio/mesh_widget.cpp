/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_widget.cpp
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
#include "mesh_widget.h"
#include <QOpenGLContext>
#include <QUuid>
#include <glm/gtc/matrix_transform.hpp>
#include "glmesh/kernel/cpu_to_gpu.h"
#include "glmesh/kernel/core/cpu_rectangle.h"
#include "glmesh/kernel/gl/gl_api_init.h"
#include "glmesh/kernel/gl/gl_debug.h"
#include "glmesh/kernel/gl/gl_triangle_mesh.h"
#include "glmesh/kernel/io/mesh_loader.h"
#include "glmesh/kernel/gl/gpu_triangle_mesh.h"
#include "glmesh/kernel/gl/gpu_rectangle.h"
#include "glmesh/kernel/gl/gl_rectangle.h"
#include "glmesh/kernel/core/cpu_bkg.h"
#include "glmesh/kernel/gl/gpu_bkg.h"
#include "glmesh/kernel/gl/gl_bkg.h"
#include "common.h"
#include "shader_program_manager.h"
#include "app_log.h"

namespace
{
    void* qtGetGlProcAddress(const char* name)
    {
        auto* ctx = QOpenGLContext::currentContext();
        if (!ctx) {
            return nullptr;
        }
        return reinterpret_cast<void*>(ctx->getProcAddress(name));
    }

    const char* kMeshVertexShader = R"(

        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec3 aColor;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProj;
        uniform mat3 uNormalMatrix;

        out vec3 vNormal;
        out vec3 vColor;

        void main()
        {
            gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);            
            vNormal = uNormalMatrix * aNormal; 
            vColor = aColor;
        }
    )";

    const char* kMeshFragmentShader = R"(

        #version 330 core
        in vec3 vNormal;
        in vec3 vColor;

        out vec4 FragColor;

        uniform vec3 uObjectColor;  
        uniform bool uUseVertexColor;    
        uniform bool uUseAmbient;
        uniform float uAmbientFactor;
        uniform vec3 uAmbientLightColor; 
        uniform bool uUseDiffuse;
        uniform vec3 uDiffuseLightColor; 
        uniform vec3 uLightDir;  

        void main()
        {
            vec3 baseColor = uUseVertexColor ? vColor : uObjectColor;

            vec3 ambient = vec3(0.0);
            if(uUseAmbient){
                ambient = uAmbientFactor * uAmbientLightColor * baseColor;
            }
            vec3 N = normalize(vNormal);
            vec3 L = normalize(-uLightDir);
            float diff = max(dot(N, L), 0.0);
            vec3 diffuse = vec3(0.0);
            if(uUseDiffuse){
                diffuse = diff * uDiffuseLightColor * baseColor;
            }
            vec3 resultClr = ambient + diffuse;
            FragColor = vec4(resultClr, 1.0);
        }
    )";

    const char* kBgVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec2 a_position;
        layout(location = 1) in vec3 a_color;
        out vec3 v_color;
        void main()
        {
            v_color = a_color;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";

    const char* kBgFrameShader = R"(
        #version 330 core
        in vec3 v_color;
        out vec4 frag_color;
        void main()
        {
            frag_color = vec4(v_color, 1.0);
        }
    )";
}

MeshWidget::MeshWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

MeshWidget::~MeshWidget()
{
    makeCurrent();
    renderable_objects_.clear();
    ShaderProgramManager::Inst().destory();
    doneCurrent();
}

void MeshWidget::initializeGL()
{
    auto* ctx = QOpenGLContext::currentContext();
    Q_ASSERT(ctx);

    const bool ok = glmesh::InitializeGLProcResolver(&qtGetGlProcAddress);
    Q_ASSERT(ok);

    glmesh::gl::EnableDebugOutput();

    auto mesh_shader_program = std::make_unique<glmesh::ShaderProgram>();
    try{
        mesh_shader_program->createFromSource(kMeshVertexShader, kMeshFragmentShader);
    }catch(const std::exception& e){
        APP_LOG_ERROR("exception:{}", std::string(e.what()));
        return;
    }
    
    ShaderProgramManager::Inst().addProgram(SPT_MESH, std::move(mesh_shader_program));

    initGradientBackground();

    gl_initialized_ = true;
}

QString MeshWidget::addMesh(const glmesh::GpuTriangleMesh& mesh_data, const glmesh::Bounds3D& bounds, UpdateError* out_err)
{
    if(!gl_initialized_){
        if(out_err){
            *out_err = UpdateError::NotInitialized;
        }
        return "";
    }

    makeCurrent();
    
    auto gl_mesh = std::make_shared<glmesh::GLTriangleMesh>();
    gl_mesh->upload(mesh_data, GL_STATIC_DRAW);

    QString mesh_uid = QUuid::createUuid().toString();

    {
        std::lock_guard lock(renderable_objects_mutex_);
        RenderableObject mesh_ren_obj;
        mesh_ren_obj.uid = mesh_uid;
        mesh_ren_obj.drawable = gl_mesh;
        mesh_ren_obj.bounds = bounds;
        mesh_ren_obj.material.shader_prog_id = SPT_MESH;
        mesh_ren_obj.material.light_dir = glm::normalize(glm::vec3(-0.0f, -0.0f, -1.0f));
        //mesh_ren_obj.material.ambient = 0.7f;
        renderable_objects_.insert({mesh_uid, std::move(mesh_ren_obj)});
    }

    doneCurrent();
    
    update(); 

    if(out_err){
        *out_err = UpdateError::None;
    }
    return mesh_uid;
}

void MeshWidget::removeMesh(const QString &mesh_uid)
{
    {
        std::lock_guard lock(renderable_objects_mutex_);
        if(renderable_objects_.contains(mesh_uid)){
            renderable_objects_.erase(mesh_uid);
        }else{
            APP_LOG_WARN("No such mesh with uid:{}", QStrToLogStr(mesh_uid));
        }        
    }
    update();
}

void MeshWidget::setMeshVisible(const QString &uid, bool visible)
{
    {
        std::lock_guard lock(renderable_objects_mutex_);
        if(renderable_objects_.contains(uid)){
            renderable_objects_[uid].visible = visible;
        }else{
            APP_LOG_WARN("No such mesh with uid:{}", QStrToLogStr(uid));
        }
    }
    update();    
}

void MeshWidget::setMeshRenderMode(const QString &uid, MeshRenderMode mode)
{
    {
        std::lock_guard lock(renderable_objects_mutex_);
        if(renderable_objects_.contains(uid)){
            renderable_objects_[uid].material.render_mode = mode;
        }else{
            APP_LOG_WARN("No such mesh with uid:{}", QStrToLogStr(uid));
        }
    }
    update();  
}

void MeshWidget::setActiveMesh(const QString &mesh_uid)
{
    if(!isValidMesh(mesh_uid)){
        APP_LOG_WARN("No such mesh with uid:{}", QStrToLogStr(mesh_uid));
        return;
    }
    current_active_mesh_uid_ = mesh_uid;
    {
        std::lock_guard lock(renderable_objects_mutex_);
        handleMeshBoundsChanged(renderable_objects_[mesh_uid].bounds);
    }
    APP_LOG_TRACE("Active mesh uid:{} updated!", QStrToLogStr(mesh_uid));
}

bool MeshWidget::isValidMesh(const QString &uid) const
{
    return renderable_objects_.contains(uid);
}

void MeshWidget::setAmbientLightEnabled(bool enabled)
{
    ambient_light_on_ = enabled;
    update();
}

void MeshWidget::resizeGL(int w, int h)
{
    ::glViewport(0, 0, w, h);
    active_camera_.setViewport(w, h);
}

void MeshWidget::paintGL()
{
    ::glEnable(GL_DEPTH_TEST);
    ::glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawGradientBackground();
    drawRenderableObjects();
}

void MeshWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton) {
        ball_rotator_.onStartRotationEvent(event, size());
    }
}

void MeshWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton) {
        ball_rotator_.onUpdateMousePos(event, size());
        active_camera_.setRotation(ball_rotator_.getRotationMat());
        update(); // 触发重绘
    }    
}

void MeshWidget::wheelEvent(QWheelEvent* event)
{
    active_camera_.zoomByWheelDelta(event->angleDelta().y());
    update(); // 仅在视角变化时要求重绘
}

void MeshWidget::drawGradientBackground()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    ShaderProgramManager::Inst().getProgram(SPT_BACKGROUND)->use();
    gl_bkg_->draw();

    glmesh::ShaderProgram::UnuseAny();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void MeshWidget::drawRenderableObjects()
{
    std::lock_guard lock(renderable_objects_mutex_);
    if(renderable_objects_.empty()){
        return;
    }
    auto view = active_camera_.viewMatrix();
    auto model = active_camera_.modelCenterMatrix();
    auto proj = active_camera_.projectionMatrix();
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));

    for(auto& kv : renderable_objects_){
        auto& ren_obj = kv.second;
        if(!ren_obj.visible || !ren_obj.drawable || ren_obj.material.shader_prog_id < 0){
            continue;
        }
        ren_obj.material.bind();
        auto shader_prog = ShaderProgramManager::Inst().getProgram(ren_obj.material.shader_prog_id);
        if(shader_prog == nullptr){
            continue;
        }
        shader_prog->setMat4("uModel", model);
        shader_prog->setMat4("uView", view);
        shader_prog->setMat4("uProj", proj);
        shader_prog->setMat3("uNormalMatrix", normal_matrix);

        shader_prog->setBool("uUseAmbient", ambient_light_on_);
        shader_prog->setFloat("uAmbientFactor", ambient_factor_);
        shader_prog->setVec3("uAmbientLightColor", ambient_light_color_);
        shader_prog->setBool("uUseDiffuse", diffuse_light_on_);
        shader_prog->setVec3("uDiffuseLightColor", diffuse_light_color_);

        shader_prog->setVec3("uLightDir", active_camera_.forward());

        switch(ren_obj.material.render_mode){
            case MeshRenderMode::Facet:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;

            case MeshRenderMode::Wireframe:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(ren_obj.material.line_width);
                break;

            case MeshRenderMode::Points:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                glPointSize(ren_obj.material.point_size);
                break;
            
            default:
                break;
        }
        ren_obj.drawable->draw();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glPointSize(1.0f);
}

void MeshWidget::handleMeshBoundsChanged(const glmesh::Bounds3D& bounds)
{
    active_camera_.fitBounds(bounds.center, bounds.radius);
    ball_rotator_.reset();
    active_camera_.setRotation(ball_rotator_.getRotationMat());
}

void MeshWidget::initGradientBackground()
{
    glmesh::CpuBkg cpu_bkg;
    cpu_bkg.left_top_vertex.position = {-1.0f,  1.0f};
    cpu_bkg.left_top_vertex.color = {0.015f, 0.018f, 0.055f};
    cpu_bkg.right_top_vertex.position = {1.0f,  1.0f};
    cpu_bkg.right_top_vertex.color = {0.015f, 0.018f, 0.055f};
    cpu_bkg.right_bottom_vertex.position = {1.0f,  -1.0f};
    cpu_bkg.right_bottom_vertex.color = {0.42f, 0.43f, 0.95f};
    cpu_bkg.left_bottom_vertex.position = {-1.0f,  -1.0f};
    cpu_bkg.left_bottom_vertex.color = {0.42f, 0.43f, 0.95f};
    auto gpu_bkg = glmesh::ToGpuBkg(cpu_bkg);    
    gl_bkg_ = std::make_unique<glmesh::GLBkg>();
    gl_bkg_->upload(gpu_bkg, GL_STATIC_DRAW);

    auto bkg_shader_prog = std::make_unique<glmesh::ShaderProgram>();
    bkg_shader_prog->createFromSource(kBgVertexShader, kBgFrameShader);
    ShaderProgramManager::Inst().addProgram(SPT_BACKGROUND, std::move(bkg_shader_prog));
}

void MeshWidget::setAmbientLight(const QColor& color, double factor)
{
    if(ambient_light_on_){
        ambient_light_color_ = glm::vec3(color.redF(), color.greenF(), color.blueF());
        ambient_factor_ = std::clamp(factor, 0.0, 1.0);
        update(); 
    }    
}

void MeshWidget::setDiffuseLightEnabled(bool enabled)
{
    diffuse_light_on_ = enabled;
    update();
}

void MeshWidget::setDiffuseLightColor(const QColor& color)
{
    if(diffuse_light_on_){
        diffuse_light_color_ = glm::vec3(color.redF(), color.greenF(), color.blueF());
        update(); 
    }    
}

void MeshWidget::setLightDirection(const glm::vec3& dir)
{
    light_dir_ = dir;
    update();
}