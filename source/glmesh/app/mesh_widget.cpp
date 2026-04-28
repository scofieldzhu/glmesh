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
#include <glm/gtc/matrix_transform.hpp>
#include "glmesh/kernel/cpu_to_gpu.h"
#include "glmesh/kernel/gl/gl_api_init.h"
#include "glmesh/kernel/gl/gl_debug.h"
#include "glmesh/kernel/gl/gl_triangle_mesh.h"
#include "glmesh/kernel/io/mesh_loader.h"
#include "glmesh/kernel/gl/gpu_triangle_mesh.h"
#include "glmesh/kernel/glmesh_log.h"

static const char* kMeshVertexShader = R"(
    #version 330 core

    layout (location = 0) in vec3 aPosition;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec3 aColor;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProj;

    out vec3 vNormal;
    out vec3 vColor;

    void main()
    {
        gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
        vNormal = mat3(transpose(inverse(uModel))) * aNormal;
        vColor = aColor;
    }
)";

static const char* kMeshFragmentShader = R"(
    #version 330 core

    in vec3 vNormal;
    in vec3 vColor;

    uniform vec3 uLightDir;
    uniform float uAmbient;

    out vec4 FragColor;

    void main()
    {
        vec3 N = normalize(vNormal);
        vec3 L = normalize(-uLightDir);

        float lambert = max(dot(N, L), 0.0);
        vec3 color = vColor * (uAmbient + lambert);

        FragColor = vec4(color, 1.0);
    }
)";

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
}

MeshWidget::MeshWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

MeshWidget::~MeshWidget()
{
    makeCurrent();
    renderable_objects_.clear();
    shader_.destroy(); 
    doneCurrent();
}

void MeshWidget::initializeGL()
{
    auto* ctx = QOpenGLContext::currentContext();
    Q_ASSERT(ctx);

    const bool ok = glmesh::InitializeGLProcResolver(&qtGetGlProcAddress);
    Q_ASSERT(ok);

    glmesh::gl::EnableDebugOutput();

    ::glEnable(GL_DEPTH_TEST);
    ::glClearColor(0.10f, 0.12f, 0.15f, 1.0f);

    shader_.createFromSource(kMeshVertexShader, kMeshFragmentShader);

    is_gl_initialized_ = true;
}

bool MeshWidget::updateMesh(const glmesh::GpuTriangleMesh& mesh_data, UpdateError* out_err)
{
    if(!is_gl_initialized_){
        if(out_err){
            *out_err = UpdateError::NotInitialized;
        }
        return false;
    }

    makeCurrent();
    
    auto gl_mesh = std::make_shared<glmesh::GLTriangleMesh>();
    gl_mesh->upload(mesh_data, GL_STATIC_DRAW);

    RenderableObject ren_obj;
    ren_obj.drawable = gl_mesh;
    ren_obj.material.shader = &shader_;
    ren_obj.material.light_dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    ren_obj.material.ambient = 0.7f;
    renderable_objects_.push_back(std::move(ren_obj));

    doneCurrent();
    
    update(); 

    if(out_err){
        *out_err = UpdateError::None;
    }
    return true;
}

void MeshWidget::resizeGL(int w, int h)
{
    ::glViewport(0, 0, w, h);
}

void MeshWidget::paintGL()
{
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(renderable_objects_.empty()){
        return;
    }
    // glm::mat4 model(1.0f);
    // model = glm::scale(model, glm::vec3(0.01f)); 
    // model = glm::rotate(model, glm::radians(rotation_x_), glm::vec3(1.0f, 0.0f, 0.0f));
    // model = glm::rotate(model, glm::radians(rotation_y_), glm::vec3(0.0f, 1.0f, 0.0f));

    // glm::mat4 view = glm::lookAt(
    //     glm::vec3(0.0f, 0.0f, camera_distance_),  // eye 加入了滚轮缩放控制
    //     glm::vec3(0.0f, 0.0f, 0.0f),              // center
    //     glm::vec3(0.0f, 1.0f, 0.0f)               // up
    // );

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -camera_distance_));

    GLMESH_LOG_TRACE("camera_distance_:{}", camera_distance_);

    // 直接将四元数转换为旋转矩阵
    glm::mat4 model = glm::mat4_cast(model_rotation_);

    const float aspect = height() > 0 ? static_cast<float>(width()) / static_cast<float>(height()) : 1.0f;
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);

    for(auto ren_obj : renderable_objects_){
        if(!ren_obj.visible || !ren_obj.drawable){
            continue;
        }
        ren_obj.material.bind();
        ren_obj.material.shader->setMat4("uModel", model);
        ren_obj.material.shader->setMat4("uView", view);
        ren_obj.material.shader->setMat4("uProj", proj);
        ren_obj.drawable->draw();
    }
}

void MeshWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        last_arcball_vec_ = getArcballVector(event->pos());
    }
}

void MeshWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 获取当前鼠标在球体上的三维向量
        glm::vec3 current_arcball_vec = getArcballVector(event->pos());
        
        // 计算两个向量的夹角余弦值
        float dot_prod = glm::dot(last_arcball_vec_, current_arcball_vec);
        // 防止浮点数精度问题导致 acos 越界返回 NaN
        dot_prod = glm::clamp(dot_prod, -1.0f, 1.0f); 
        
        // 计算旋转角度
        float angle = std::acos(dot_prod);
        
        if (angle > 1e-5f) { // 如果确实发生了移动
            // 通过叉乘得到旋转轴 (右手定则)
            glm::vec3 axis = glm::cross(last_arcball_vec_, current_arcball_vec);
            axis = glm::normalize(axis);
            
            // 构建这一次移动产生的旋转四元数
            glm::quat delta_rotation = glm::angleAxis(angle, axis);
            
            // 将本次旋转叠加到总旋转上 (注意乘法顺序)
            model_rotation_ = delta_rotation * model_rotation_;
        }
        
        // 更新记录，用于下一帧计算
        last_arcball_vec_ = current_arcball_vec;
        
        update(); // 触发重绘
    }    
}

void MeshWidget::wheelEvent(QWheelEvent* event)
{
    // 滚轮控制远近
    float scroll = event->angleDelta().y() / 120.0f;
    camera_distance_ -= scroll * 0.5f;
    if (camera_distance_ < 0.1f){
        camera_distance_ = 0.1f;
    }
    update(); // 仅在视角变化时要求重绘
}

glm::vec3 MeshWidget::getArcballVector(const QPoint& pt)const
{
    // 1. 将鼠标坐标归一化到 [-1, 1] 的 NDC 空间
    // 注意：Qt的Y轴向下，OpenGL的Y轴向上，所以Y要做 1.0 - ... 的反转
    float x = (2.0f * pt.x() / width()) - 1.0f;
    float y = 1.0f - (2.0f * pt.y() / height()); 
    
    glm::vec3 P(x, y, 0.0f);
    
    // 2. 计算点到屏幕中心的距离的平方
    float OP_squared = x * x + y * y;
    
    if (OP_squared <= 1.0f) {
        // 如果点在球体内（勾股定理求 Z）
        P.z = std::sqrt(1.0f - OP_squared); 
    } else {
        // 如果点在球体外，把它拉回球面上（Z = 0）
        P = glm::normalize(P); 
    }
    
    return P;
}