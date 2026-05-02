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
        out vec3 vNormal;
        out vec3 vColor;
        void main()
        {
            gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
            vNormal = mat3(transpose(inverse(uModel))) * aNormal;
            vColor = aColor;
        }
    )";

    const char* kMeshFragmentShader = R"(
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
    mesh_shader_program_.destroy(); 
    doneCurrent();
}

void MeshWidget::initializeGL()
{
    auto* ctx = QOpenGLContext::currentContext();
    Q_ASSERT(ctx);

    const bool ok = glmesh::InitializeGLProcResolver(&qtGetGlProcAddress);
    Q_ASSERT(ok);

    glmesh::gl::EnableDebugOutput();

    mesh_shader_program_.createFromSource(kMeshVertexShader, kMeshFragmentShader);

    initGradientBackground();

    is_gl_initialized_ = true;
}

bool MeshWidget::updateMesh(const glmesh::GpuTriangleMesh& mesh_data, const glmesh::MeshBounds& bounds, UpdateError* out_err)
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
    
    RenderableObject mesh_ren_obj;
    mesh_ren_obj.drawable = gl_mesh;
    mesh_ren_obj.material.shader = &mesh_shader_program_;
    mesh_ren_obj.material.light_dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    mesh_ren_obj.material.ambient = 0.7f;
    renderable_objects_.push_back(std::move(mesh_ren_obj));

        // 1. 设置模型居中偏移
    // 渲染时，我们要把模型从它原本的中心点平移到原点 (0,0,0)
    // 所以 model 矩阵需要加上 translate(-center_x, -center_y, -center_z)
    mesh_center_offset_ = -bounds.center;
    
    // 2. 自适应相机距离
    // 如果想要模型在视野中大小合适，可以根据它的包围球半径来设置
    // 假设 FOV 是 45 度，相机距离通常设置为半径的 2 到 3 倍
    camera_distance_ = bounds.radius * 2.0f;
    
    // 3. 设置滚轮缩放限制
    // 防止滚轮滑得太近直接穿模，或者滑得太远看不见
    min_camera_distance_ = bounds.radius * 1.2f;  // 至少离模型表面一点点
    max_camera_distance_ = bounds.radius * 10.0f; // 最远不超过半径的10倍
    
    // 4. 重置相机的旋转状态
    // 重置 Arcball 或者四元数等状态，保证每次换模型都是正对的
    // ...

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
    ::glEnable(GL_DEPTH_TEST);
    ::glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawGradientBackground();
    drawRenderableObjects();
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
    // // 滚轮控制远近
    // float scroll = event->angleDelta().y() / 120.0f;
    // camera_distance_ -= scroll * 0.5f;
    // if (camera_distance_ < 0.1f){
    //     camera_distance_ = 0.1f;
    // }

    float scroll_amount = event->angleDelta().y() / 120.0f;
    
    // 缩放步长也可以跟模型尺寸挂钩，大模型缩放快，小模型缩放慢
    float step = (max_camera_distance_ - min_camera_distance_) * 0.05f; 
    
    camera_distance_ -= scroll_amount * step;
    
    // 应用最小和最大距离限制，防止穿模和丢失
    if (camera_distance_ < min_camera_distance_) {
        camera_distance_ = min_camera_distance_;
    }
    if (camera_distance_ > max_camera_distance_) {
        camera_distance_ = max_camera_distance_;
    }
    update(); // 仅在视角变化时要求重绘
}

void MeshWidget::drawGradientBackground()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    bg_shader_program_.use();

    gl_bkg_->draw();

    glmesh::ShaderProgram::UnuseAny();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void MeshWidget::drawRenderableObjects()
{
    if(renderable_objects_.empty()){
        return;
    }
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -camera_distance_));
    // 加上你的旋转矩阵 (鼠标交互产生的)
    view = view * glm::mat4_cast(model_rotation_); 

    //APP_LOG_TRACE("camera_distance_:{}", camera_distance_);

    // 直接将四元数转换为旋转矩阵
    //glm::mat4 model = glm::mat4_cast(model_rotation_);

    // Model 矩阵：将模型自身平移，使其包围盒中心对准原点
    glm::mat4 model = glm::translate(glm::mat4(1.0f), mesh_center_offset_);

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

    bg_shader_program_.createFromSource(kBgVertexShader, kBgFrameShader);
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