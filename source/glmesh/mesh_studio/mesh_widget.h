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
#include "common.h"
#include "glmesh/kernel/gl/gl_bkg.h"
#include "renderable_object.h"
#include "arcball_rotator.h"
#include "glmesh/render/camera.h"

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
    QString addMesh(const glmesh::GpuTriangleMesh& mesh_data, const glmesh::Bounds3D& mb, UpdateError* out_err = nullptr);
    void removeMesh(const QString& mesh_uid);
    void setMeshVisible(const QString& uid, bool visible);
    void setMeshRenderMode(const QString& uid, MeshRenderMode mode);
    void setActiveMesh(const QString& mesh_uid);
    QString activeMesh()const{ return current_active_mesh_uid_; }
    bool isValidMesh(const QString& uid)const;
    void setAmbientLightEnabled(bool enabled);
    void setAmbientLight(const QColor& color, double factor);
    QColor ambientLightColor()const{ return ToColor(ambient_light_color_); }
    void setDiffuseLightEnabled(bool enabled);
    void setDiffuseLightColor(const QColor& color);
    QColor diffuseLightColor()const{ return ToColor(diffuse_light_color_); }
    void setLightDirection(const glm::vec3& dir);

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
    bool gl_initialized_ = false;
    ArcBallRotator ball_rotator_;
    std::unique_ptr<glmesh::GLBkg> gl_bkg_;
    QString current_active_mesh_uid_;
    glmesh::Camera active_camera_;

    bool ambient_light_on_ = true;
    double ambient_factor_ = 1.0f;
    glm::vec3 ambient_light_color_{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse_light_color_{1.0f, 0.0f, 0.0f};
    bool diffuse_light_on_ = true;
    // 光照方向 (默认从右上方往左下方打光，世界坐标系)
    glm::vec3 light_dir_{1.0f, 1.0f, 1.0f}; 
};

#endif
