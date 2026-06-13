/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: draw_polyline_interaction.h
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
#ifndef __draw_polyine_interaction_h__
#define __draw_polyine_interaction_h__

#include "mouse_interaction.h"
#include "glmesh/kernel/glmesh_kernel_typedef.h"
#include "glmesh/kernel/gl/gl_polyline.h"
#include <memory>

class DrawPolylineInteraction : public IMouseInteraction
{
public:
    void onMousePress(QMouseEvent* event, const MouseInteractionContext& ctx) override;
    void onMouseMove(QMouseEvent* event, const MouseInteractionContext& ctx) override;
    void onWheel(QWheelEvent* event, const MouseInteractionContext& ctx) override;
    void onKeyPress(QKeyEvent* event, const MouseInteractionContext& ctx) override;

    /**
     * @brief 完成折线绘制，返回绘制的点集
     * @return 折线顶点集合
     */
    std::vector<glm::vec3> finishDrawing();

    /**
     * @brief 取消折线绘制，清空已拾取的点
     */
    void cancelDrawing();

    /**
     * @brief 撤销最后一个点
     */
    void undoLastPoint();

    /**
     * @brief 获取当前已拾取的点数量
     */
    size_t pointCount() const { return polyline_points_.size(); }

private:
    std::vector<glm::vec3> polyline_points_;
    bool preview_mode_enabled_ = true;  // 是否启用预览模式
};

#endif