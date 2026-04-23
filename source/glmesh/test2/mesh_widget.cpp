
#include "mesh_widget.h"


#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>

#include "glmesh/kernel/renderer.h"
#include "glmesh/kernel/core/cpu_to_gpu.h"
#include "glmesh/kernel/gl/gl_api_init.h"
#include "glmesh/kernel/gl/gl_debug.h"
#include "glmesh/kernel/io/mesh_loader.h"

#include <QOpenGLContext>
#include <QDebug>

using namespace glmesh;

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
    connect(&frame_timer_, &QTimer::timeout, this, QOverload<>::of(&QOpenGLWidget::update));
    frame_timer_.start(16); // ~60 FPS
}

MeshWidget::~MeshWidget()
{
    makeCurrent();
    mesh_.reset();
    doneCurrent();
}

void MeshWidget::initializeGL()
{
    auto* ctx = QOpenGLContext::currentContext();
    Q_ASSERT(ctx);

    const bool ok = InitializeGLProcResolver(&qtGetGlProcAddress);
    Q_ASSERT(ok);

    ::glmesh::gl::EnableDebugOutput();

qDebug() << "GL_VERSION ="
         << reinterpret_cast<const char*>(::glGetString(GL_VERSION));
qDebug() << "GLSL_VERSION ="
         << reinterpret_cast<const char*>(::glGetString(GL_SHADING_LANGUAGE_VERSION));

    ::glEnable(GL_DEPTH_TEST);
    ::glClearColor(0.10f, 0.12f, 0.15f, 1.0f);

    shader_.createFromSource(kMeshVertexShader, kMeshFragmentShader);

    material_.shader = &shader_;
    material_.light_dir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    material_.ambient = 0.2f;

    mesh_ = std::make_unique<GLTriangleMesh>(
        LoadPlyRenderableMesh("E:/Code/glmesh/CFMC-46-009.ply")
    );

    elapsed_timer_.start();
}

void MeshWidget::resizeGL(int w, int h)
{
    ::glViewport(0, 0, w, h);
}

void MeshWidget::paintGL()
{
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mesh_)
    {
        return;
    }

    const float t = 0.001f * static_cast<float>(elapsed_timer_.elapsed());

    glm::mat4 model(1.0f);
    model = glm::scale(model, glm::vec3(0.01f)); // 按你的模型大小改
    model = glm::rotate(model, t * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),  // eye
        glm::vec3(0.0f, 0.0f, 0.0f),  // center
        glm::vec3(0.0f, 1.0f, 0.0f)   // up
    );

    const float aspect = height() > 0 ? static_cast<float>(width()) / static_cast<float>(height()) : 1.0f;
    glm::mat4 proj = glm::perspective(
        glm::radians(45.0f),
        aspect,
        0.1f,
        100.0f
    );

    DrawMesh(*mesh_, material_, model, view, proj);
}