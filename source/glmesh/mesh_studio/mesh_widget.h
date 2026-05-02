/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_widget.h
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
#ifndef __mesh_widget_h__
#define __mesh_widget_h__

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glmesh/kernel/gl/shader_program.h"
#include "glmesh/kernel/gl/gl_bkg.h"
#include "renderable_object.h"

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
    bool updateMesh(const glmesh::GpuTriangleMesh& mesh_data, const glmesh::MeshBounds& mb, UpdateError* outError = nullptr);
    explicit MeshWidget(QWidget* parent = nullptr);
    ~MeshWidget() override;

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void initGradientBackground();
    void drawGradientBackground();
    void drawRenderableObjects();
    glm::vec3 getArcballVector(const QPoint& pt)const;
    std::vector<RenderableObject> renderable_objects_;
    glmesh::ShaderProgram mesh_shader_program_;
    float camera_distance_ = 100.0f;
    bool is_gl_initialized_ = false;
    glm::quat model_rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // 记录累积的旋转（四元数）
    glm::vec3 last_arcball_vec_;   
    glm::vec3 mesh_center_offset_;
    float min_camera_distance_ = 0.0;
    float max_camera_distance_ = 0.0;
    glmesh::ShaderProgram bg_shader_program_;
    std::unique_ptr<glmesh::GLBkg> gl_bkg_;
};

#endif
