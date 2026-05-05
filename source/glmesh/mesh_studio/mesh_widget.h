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
#include <mutex>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glmesh/kernel/gl/gl_bkg.h"
#include "shader_program_manager.h"
#include "renderable_object.h"
#include "arcball_rotator.h"

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
    QString addMesh(const glmesh::GpuTriangleMesh& mesh_data, const glmesh::Bounds3D& mb, UpdateError* outError = nullptr);
    void removeMesh(const QString& mesh_uid);
    void setMeshVisible(const QString& uid, bool visible);
    void setActiveMesh(const QString& mesh_uid);
    bool isValidMesh(const QString& uid)const;
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
    void handleMeshBoundsChanged(const glmesh::Bounds3D& bounds);
    std::mutex renderable_objects_mutex_;
    std::unordered_map<QString, RenderableObject> renderable_objects_;
    float camera_distance_ = 100.0f;
    bool gl_initialized_ = false;
    ArcBallRotator ball_rotator_;
    glm::vec3 mesh_center_offset_;
    float min_camera_distance_ = 0.0;
    float max_camera_distance_ = 0.0;
    std::unique_ptr<glmesh::GLBkg> gl_bkg_;
    ShaderProgramManager program_mgr_;
    QString current_active_mesh_uid_;
};

#endif
