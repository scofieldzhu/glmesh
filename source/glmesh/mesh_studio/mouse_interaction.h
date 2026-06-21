/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: mouse_interaction.h
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
#ifndef __mouse_interaction_h__
#define __mouse_interaction_h__

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <functional>

namespace glmesh {
    class Camera;
    class GLTrackballGizmo;
    class Renderer;
}

class MeshWidget;
class ArcBallRotator;
class SceneManager;

struct MouseInteractionContext
{
    MeshWidget* mesh_widget_ = nullptr;
    glmesh::Camera* camera = nullptr;
    ArcBallRotator* ball_rotator = nullptr;
    glmesh::GLTrackballGizmo* gizmo = nullptr;
    int* hovered_gizmo_axis = nullptr;
    int widget_width = 0;
    int widget_height = 0;
    std::function<void()> request_update_func;
    SceneManager* scene_manager = nullptr;          // 场景管理器
    glmesh::Renderer* renderer = nullptr;           // 渲染器（坐标转换/拾取）
    std::function<float(int,int)> readDepthFunc;   // 读取深度缓冲（需 GL 上下文）
    std::function<void(std::function<void()>)> runInGLContext;  // 在 GL 上下文中执行代码块
};

class IMouseInteraction
{
public:
    virtual void onMousePress(QMouseEvent* event, const MouseInteractionContext& ctx) = 0;
    virtual void onMouseMove(QMouseEvent* event, const MouseInteractionContext& ctx) = 0;
    virtual void onWheel(QWheelEvent* event, const MouseInteractionContext& ctx) = 0;
    virtual void onKeyPress(QKeyEvent* event, const MouseInteractionContext& ctx){ /* 默认空实现 */ }
    virtual ~IMouseInteraction() = default;
};

#endif