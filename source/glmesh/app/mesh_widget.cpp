#include "mesh_widget.h"
#include <QOpenGLContext>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include "glmesh/kernel/renderer.h"
#include "glmesh/kernel/cpu_to_gpu.h"
#include "glmesh/kernel/gl/gl_api_init.h"
#include "glmesh/kernel/gl/gl_debug.h"
#include "glmesh/kernel/io/mesh_loader.h"

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
    mesh_.reset();
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

    material_.shader = &shader_;
    material_.light_dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    material_.ambient = 0.2f;

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
    
    if(!mesh_) {
        mesh_ = std::make_unique<glmesh::GLTriangleMesh>();
    }
    // 此处调用你现有的 CPU -> GPU 转换逻辑
    // 假设你底层有类似 LoadFromMemory 的函数：
    // mesh_->load(data.positions, data.normals, data.colors, data.indices);
    //LoadPlyRenderableMesh("E:/Code/glmesh/CFMC-46-009.ply", *mesh_);

    mesh_->upload(mesh_data, GL_STATIC_DRAW);

    doneCurrent();
    
    // 数据更新后请求重绘一次
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

    if(!mesh_ || !mesh_->valid()){
        return;
    }
    // 【优化】旋转逻辑由计时器改为鼠标控制
    glm::mat4 model(1.0f);
    model = glm::scale(model, glm::vec3(0.01f)); 
    // 应用鼠标旋转
    model = glm::rotate(model, glm::radians(rotation_x_), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation_y_), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, camera_distance_),  // eye 加入了滚轮缩放控制
        glm::vec3(0.0f, 0.0f, 0.0f),              // center
        glm::vec3(0.0f, 1.0f, 0.0f)               // up
    );

    const float aspect = height() > 0 ? static_cast<float>(width()) / static_cast<float>(height()) : 1.0f;
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    DrawMesh(*mesh_, material_, model, view, proj);
}

void MeshWidget::mousePressEvent(QMouseEvent* event)
{
    last_mouse_pos_ = event->pos();
}

void MeshWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton){
        int dx = event->x() - last_mouse_pos_.x();
        int dy = event->y() - last_mouse_pos_.y();
        
        rotation_y_ += dx * 0.5f;
        rotation_x_ += dy * 0.5f;
        
        last_mouse_pos_ = event->pos();
        update(); // 仅在数据变化时要求重绘
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