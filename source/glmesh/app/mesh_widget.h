#pragma once

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include "glmesh/kernel/gl/shader_program.h"
#include "glmesh/kernel/gl/gpu_triangle_mesh.h"
#include "glmesh/kernel/material.h"

class MeshWidget : public QOpenGLWidget 
{
    Q_OBJECT
public:
    enum class UpdateError 
    {
        None,
        EmptyData,
        NotInitialized
    };
    // 核心接口：主线程完成 IO 加载后，调用此接口将数据喂给 GPU
    bool updateMesh(const glmesh::GpuTriangleMesh& mesh_data, UpdateError* outError = nullptr);
    explicit MeshWidget(QWidget* parent = nullptr);
    ~MeshWidget() override;

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    std::unique_ptr<glmesh::GLTriangleMesh> mesh_;
    glmesh::ShaderProgram shader_;
    glmesh::Material material_;
    QPoint last_mouse_pos_;
    float camera_distance_ = 3.0f;
    float rotation_x_ = 0.0f;
    float rotation_y_ = 0.0f;
    bool is_gl_initialized_ = false;
};
