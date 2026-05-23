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
#include "glmesh/kernel/gl/gl_trackball_gizmo.h"
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
        out vec3 vFragPos; // 传给片元着色器的世界坐标

        void main()
        {
            vec4 worldPos = uModel * vec4(aPosition, 1.0);
            vFragPos = worldPos.xyz; // 记录世界坐标
            gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);            
            vNormal = uNormalMatrix * aNormal; 
            vColor = aColor;
        }
    )";

    const char* kMeshFragmentShader = R"(

        #version 330 core
        in vec3 vNormal;
        in vec3 vColor;
        in vec3 vFragPos; // 接收世界坐标

        out vec4 FragColor;

        uniform vec3 uObjectColor;  
        uniform bool uUseVertexColor;    
        uniform float uAmbientFactor;
        uniform vec3 uAmbientLightColor; 
        uniform bool uUseDiffuse;
        uniform vec3 uDiffuseLightColor; 
        uniform vec3 uLightDir;  

        // 【新增】相机位置，用于算高光视线角度
        uniform vec3 uViewPos;

        void main()
        {
            vec3 baseColor = uUseVertexColor ? vColor : uObjectColor;
            vec3 ambient = uAmbientFactor * uAmbientLightColor * baseColor;

            vec3 diffuse = vec3(0.0);
            vec3 specular = vec3(0.0); // 【新增】高光颜色
            if(uUseDiffuse){
                vec3 N;
                // 关键防御逻辑：如果顶点法线无效(长度接近0)，则实时生成 Flat 法线
                if (length(vNormal) < 0.01) {
                    vec3 fdx = dFdx(vFragPos);
                    vec3 fdy = dFdy(vFragPos);
                    N = normalize(cross(fdx, fdy));
                } else {
                    N = normalize(vNormal);
                }
                // L 向量：光源方向取反（指向光源）
                // Offset light direction slightly to avoid headlamp effect
                vec3 L = normalize(-uLightDir);
                // 为了防止在模型背面变全黑，可以使用 abs(dot) 实现双面光照 (Double-sided lighting)
                // 这在查看内部拓扑时非常有用
                float diff = max(dot(N, L), 0.0);
                diffuse = diff * uDiffuseLightColor * baseColor;

                // 3. 【新增】镜面高光 (Specular) - Blinn-Phong 模型
                vec3 viewDir = normalize(uViewPos - vFragPos); // 从片元指向相机的视线
                vec3 halfVector = normalize(L + viewDir);      // 光线与视线的半程向量
                
                // 只有被光照到的面（diff > 0）才有高光
                if (diff > 0.0) {
                    // 32.0 是反光度(Shininess)，值越大光斑越小越锐利(像金属)，越小光斑越散(像塑料)
                    float spec = pow(max(dot(N, halfVector), 0.0), 32);
                    // 高光一般用光源颜色（这里让它稍微偏白一点，效果更好）
                    specular = spec * uDiffuseLightColor; 
                }
            }

            FragColor = vec4(ambient + diffuse + specular, 1.0);
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

    const char* kGizmoVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 uMVP;
        void main(){ gl_Position = uMVP * vec4(aPos, 1.0); }
    )";

    const char* kGizmoFragmentShader = R"(
        #version 330 core
        uniform vec3 uColor;
        out vec4 FragColor;
        void main(){ FragColor = vec4(uColor, 1.0); }
    )";
}

MeshWidget::MeshWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);
}

MeshWidget::~MeshWidget()
{
    makeCurrent();
    renderable_objects_.clear();
    trackball_gizmo_.reset();
    gl_bkg_.reset();
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
    initTrackballGizmo();

    gl_initialized_ = true;
}

QString MeshWidget::addMesh(const glmesh::GpuTriangleMesh<>& mesh_data, const glmesh::Bounds3D& bounds, UpdateError* out_err)
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
    drawTrackballGizmo();
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
        return;
    }
    int new_axis = pickGizmoAxis(event->pos());
    if(new_axis != hovered_gizmo_axis_){
        hovered_gizmo_axis_ = new_axis;
        update();
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

        shader_prog->setFloat("uAmbientFactor", ambient_factor_);
        shader_prog->setVec3("uAmbientLightColor", ambient_light_color_);
        shader_prog->setBool("uUseDiffuse", diffuse_light_on_);        
        shader_prog->setVec3("uDiffuseLightColor", diffuse_light_color_);

        glm::mat4 inv_view    = glm::inverse(view);
        glm::vec3 cam_back    = glm::vec3(inv_view[2]); // 指向相机后方
        glm::vec3 cam_forward = -glm::vec3(inv_view[2]);
        glm::vec3 cam_up      =  glm::vec3(inv_view[1]);
        glm::vec3 cam_right   =  glm::vec3(inv_view[0]);

        // 让光源从相机的 右上方 打向模型，而不是正前方
        // 偏移系数 (0.5右, 0.5上) 可以自己微调
        // 构造一个新的光源方向：放在相机的右上方，并照向模型
        // 也就是顺着 相机右方(0.5) + 相机上方(0.5) + 相机前方(-1.0) 的混合方向打光
        glm::vec3 custom_light_dir = glm::normalize(0.5f * cam_right + 0.5f * cam_up - 1.0f * cam_back);
        shader_prog->setVec3("uLightDir", custom_light_dir);

        // 为了算高光，我们还需要把相机当前的世界坐标传给 Shader
        glm::vec3 cam_pos = glm::vec3(inv_view[3]);
        shader_prog->setVec3("uViewPos", cam_pos);

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
    ambient_light_color_ = glm::vec3(color.redF(), color.greenF(), color.blueF());
    ambient_factor_ = std::clamp(factor, 0.0, 1.0);
    update(); 
}

void MeshWidget::setDiffuseLightEnabled(bool enabled)
{
    diffuse_light_on_ = enabled;
}

void MeshWidget::setDiffuseLightColor(const QColor &color)
{
    diffuse_light_color_ = glm::vec3(color.redF(), color.greenF(), color.blueF());
    update(); 
}

void MeshWidget::setLightDirection(const glm::vec3& dir)
{
    light_dir_ = dir;
    update();
}

void MeshWidget::initTrackballGizmo()
{
    auto gizmo_shader = std::make_unique<glmesh::ShaderProgram>();
    gizmo_shader->createFromSource(kGizmoVertexShader, kGizmoFragmentShader);
    ShaderProgramManager::Inst().addProgram(SPT_GIZMO, std::move(gizmo_shader));

    trackball_gizmo_ = std::make_unique<glmesh::GLTrackballGizmo>();
    trackball_gizmo_->create(96);
}

void MeshWidget::drawTrackballGizmo()
{
    if(!trackball_gizmo_ || !trackball_gizmo_->valid()){
        return;
    }
    int w = width();
    int h = height();
    if(w <= 0 || h <= 0){
        return;
    }

    float world_radius = computeGizmoWorldRadius();
    if(world_radius <= 0.0f){
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glLineWidth(1.5f);

    // gizmo 中心位于世界原点（与 mesh 经 modelCenterMatrix 后所在位置一致）。
    glm::mat4 view = active_camera_.viewMatrix();
    glm::mat4 proj = active_camera_.projectionMatrix();
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(world_radius));
    glm::mat4 mvp = proj * view * model;

    auto* prog = ShaderProgramManager::Inst().getProgram(SPT_GIZMO);
    if(prog == nullptr){
        glLineWidth(1.0f);
        return;
    }
    prog->use();
    prog->setMat4("uMVP", mvp);

    // X=红, Y=绿, Z=蓝（与 MeshLab 一致），高亮为黄色。
    const glm::vec3 axis_colors[3] = {
        glm::vec3(0.95f, 0.25f, 0.25f),
        glm::vec3(0.25f, 0.85f, 0.25f),
        glm::vec3(0.30f, 0.45f, 0.95f)
    };
    const glm::vec3 highlight_color(1.0f, 0.85f, 0.0f);

    for(int axis = 0; axis < 3; ++axis){
        bool hi = (axis == hovered_gizmo_axis_);
        prog->setVec3("uColor", hi ? highlight_color : axis_colors[axis]);
        if(hi){
            glLineWidth(2.5f);
        }else{
            glLineWidth(1.5f);
        }
        trackball_gizmo_->drawAxis(axis);
    }

    glmesh::ShaderProgram::UnuseAny();
    glLineWidth(1.0f);
}

float MeshWidget::computeGizmoWorldRadius() const
{
    int h = height();
    if(h <= 0){
        return 0.0f;
    }
    // 目标：让三环在屏幕上的视觉半径恒为 viewport 短边的固定比例（与缩放无关）。
    // 透视投影下，世界半径 r 在屏幕上的像素半径约为 (r / depth) * (h / 2) * proj[1][1]。
    // 反推 r = pixel_radius * (2 * depth) / (h * proj[1][1])。
    // depth 取相机到 gizmo 中心的距离 = active_camera_.distance()。
    int w = width();
    float screen_short = static_cast<float>(std::min(w, h));
    float pixel_radius = 0.4f * screen_short;

    glm::mat4 proj = active_camera_.projectionMatrix();
    float p11 = proj[1][1]; // 透视: cot(fov/2); 正交: 2/height_world
    if(std::abs(p11) < 1e-6f){
        return 1.0f;
    }
    if(active_camera_.projectionType() == glmesh::Camera::ProjectionType::Perspective){
        float depth = active_camera_.distance();
        return pixel_radius * (2.0f * depth) / (static_cast<float>(h) * p11);
    }
    // 正交投影下半径与距离无关，但会跟随 orthographic_scale 缩放，
    // 此处用 proj[1][1] 推回 world 单位即可。
    return pixel_radius * 2.0f / (static_cast<float>(h) * p11);
}

int MeshWidget::pickGizmoAxis(const QPoint& pos) const
{
    if(!trackball_gizmo_ || !trackball_gizmo_->valid()){
        return -1;
    }
    int w = width();
    int h = height();
    if(w <= 0 || h <= 0){
        return -1;
    }
    float world_radius = const_cast<MeshWidget*>(this)->computeGizmoWorldRadius();
    if(world_radius <= 0.0f){
        return -1;
    }

    glm::mat4 mvp = active_camera_.projectionMatrix() *
                    active_camera_.viewMatrix() *
                    glm::scale(glm::mat4(1.0f), glm::vec3(world_radius));

    auto project = [&](const glm::vec3& wp, glm::vec2& out_screen) -> bool {
        glm::vec4 clip = mvp * glm::vec4(wp, 1.0f);
        if(clip.w <= 0.0f){
            return false;
        }
        glm::vec3 ndc(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
        out_screen.x = (ndc.x * 0.5f + 0.5f) * static_cast<float>(w);
        out_screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(h);
        return true;
    };

    const float pick_threshold_px = 8.0f;
    glm::vec2 mouse(static_cast<float>(pos.x()), static_cast<float>(pos.y()));
    int seg = trackball_gizmo_->segments();
    int best_axis = -1;
    float best_d2 = pick_threshold_px * pick_threshold_px;

    for(int axis = 0; axis < 3; ++axis){
        for(int i = 0; i < seg; ++i){
            glm::vec3 a_w = glmesh::GLTrackballGizmo::axisRingPoint(axis, i, seg);
            glm::vec3 b_w = glmesh::GLTrackballGizmo::axisRingPoint(axis, (i + 1) % seg, seg);
            glm::vec2 a_s, b_s;
            if(!project(a_w, a_s) || !project(b_w, b_s)){
                continue;
            }
            // 点到线段距离平方
            glm::vec2 ab = b_s - a_s;
            float ab_len2 = glm::dot(ab, ab);
            float t = ab_len2 > 1e-6f ? glm::clamp(glm::dot(mouse - a_s, ab) / ab_len2, 0.0f, 1.0f) : 0.0f;
            glm::vec2 closest = a_s + t * ab;
            float d2 = glm::dot(mouse - closest, mouse - closest);
            if(d2 < best_d2){
                best_d2 = d2;
                best_axis = axis;
            }
        }
    }
    return best_axis;
}
