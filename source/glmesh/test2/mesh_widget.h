#pragma once

#include <QElapsedTimer>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QTimer>

#include <glm/glm.hpp>

#include <memory>

#include "glmesh/kernel/gl/gl_triangle_mesh.h"
#include "glmesh/kernel/material.h"
#include "glmesh/kernel/gl/shader_program.h"

class MeshWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit MeshWidget(QWidget* parent = nullptr);
    ~MeshWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    glmesh::ShaderProgram shader_;
    glmesh::Material material_;
    std::unique_ptr<glmesh::GLTriangleMesh> mesh_;

    QTimer frame_timer_;
    QElapsedTimer elapsed_timer_;
};