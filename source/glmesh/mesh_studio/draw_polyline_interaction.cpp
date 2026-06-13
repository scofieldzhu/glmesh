/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: draw_polyline_interaction.cpp
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
#include "draw_polyline_interaction.h"
#include "glmesh/render/camera.h"
#include "mesh_widget.h"
#include "orbit_interaction.h"
#include "app_log.h"

void DrawPolylineInteraction::onMousePress(QMouseEvent* event, const MouseInteractionContext& ctx)
{
    if (event->button() == Qt::LeftButton) {
        // 左键点击：拾取点
        auto world_pt_opt = ctx.mesh_widget_->pickWorldPoint(event->x(), event->y());
        if (world_pt_opt) {
            polyline_points_.push_back(*world_pt_opt);

            // 更新临时折线显示
            ctx.mesh_widget_->updateTempPolyline(polyline_points_);

            APP_LOG_INFO("Polyline point {} picked: ({:.3f}, {:.3f}, {:.3f})",
                polyline_points_.size(),
                world_pt_opt->x, world_pt_opt->y, world_pt_opt->z);
        } else {
            APP_LOG_WARN("No geometry picked at screen position ({}, {})", event->x(), event->y());
        }
    } else if (event->button() == Qt::RightButton) {
        // 右键点击：完成绘制并退出模式
        if (polyline_points_.size() >= 2) {
            QString uid = ctx.mesh_widget_->finalizeTempPolyline(polyline_points_);
            if (!uid.isEmpty()) {
                APP_LOG_INFO("Polyline drawing finished with {} points, UID: {}",
                    polyline_points_.size(), uid.toStdString());
            }
        } else {
            APP_LOG_WARN("Need at least 2 points to finish polyline, canceling");
            ctx.mesh_widget_->clearTempPolyline();
        }
        polyline_points_.clear();

        // 恢复默认的 Orbit 交互模式
        ctx.mesh_widget_->setMouseInteraction(std::make_unique<OrbitInteraction>());
        APP_LOG_INFO("Exited polyline drawing mode");
    } else if (event->button() == Qt::MiddleButton) {
        // 中键：撤销最后一个点
        undoLastPoint();
        if (!polyline_points_.empty()) {
            ctx.mesh_widget_->updateTempPolyline(polyline_points_);
        } else {
            ctx.mesh_widget_->clearTempPolyline();
        }
    }
}

void DrawPolylineInteraction::onMouseMove(QMouseEvent* event, const MouseInteractionContext& ctx)
{
    // 实时预览：显示从最后一个点到鼠标位置的连线
    if (!preview_mode_enabled_ || polyline_points_.empty()) {
        return;
    }

    // 尝试拾取鼠标位置的世界坐标
    auto world_pt_opt = ctx.mesh_widget_->pickWorldPoint(event->x(), event->y());
    if (world_pt_opt) {
        // 构建预览折线：已有点 + 当前鼠标位置
        std::vector<glm::vec3> preview_points = polyline_points_;
        preview_points.push_back(*world_pt_opt);
        ctx.mesh_widget_->updateTempPolyline(preview_points);
    }
}

void DrawPolylineInteraction::onWheel(QWheelEvent* event, const MouseInteractionContext& ctx)
{
    // 折线绘制模式下禁用滚轮缩放，避免误操作
    // 用户可以按 ESC 退出绘制模式后再缩放
}

void DrawPolylineInteraction::onKeyPress(QKeyEvent* event, const MouseInteractionContext& ctx)
{
    if (event->key() == Qt::Key_Escape) {
        // ESC：取消绘制并退出
        APP_LOG_INFO("Polyline drawing canceled (ESC pressed)");
        cancelDrawing();
        ctx.mesh_widget_->clearTempPolyline();

        // 恢复默认交互
        ctx.mesh_widget_->setMouseInteraction(std::make_unique<OrbitInteraction>());
    } else if (event->key() == Qt::Key_Z && (event->modifiers() & Qt::ControlModifier)) {
        // Ctrl+Z：撤销最后一个点
        undoLastPoint();
        if (!polyline_points_.empty()) {
            ctx.mesh_widget_->updateTempPolyline(polyline_points_);
        } else {
            ctx.mesh_widget_->clearTempPolyline();
        }
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Enter：完成绘制
        if (polyline_points_.size() >= 2) {
            QString uid = ctx.mesh_widget_->finalizeTempPolyline(polyline_points_);
            if (!uid.isEmpty()) {
                APP_LOG_INFO("Polyline drawing finished with {} points (Enter pressed), UID: {}",
                    polyline_points_.size(), uid.toStdString());
            }
            polyline_points_.clear();
        } else {
            APP_LOG_WARN("Need at least 2 points to finish polyline");
        }

        // 恢复默认交互
        ctx.mesh_widget_->setMouseInteraction(std::make_unique<OrbitInteraction>());
    }
}

std::vector<glm::vec3> DrawPolylineInteraction::finishDrawing()
{
    APP_LOG_INFO("Finish drawing polyline with {} points", polyline_points_.size());
    std::vector<glm::vec3> result = std::move(polyline_points_);
    polyline_points_.clear();
    return result;
}

void DrawPolylineInteraction::cancelDrawing()
{
    APP_LOG_INFO("Cancel drawing polyline, {} points discarded", polyline_points_.size());
    polyline_points_.clear();
}

void DrawPolylineInteraction::undoLastPoint()
{
    if (!polyline_points_.empty()) {
        polyline_points_.pop_back();
        APP_LOG_INFO("Undo last point, {} points remaining", polyline_points_.size());
    } else {
        APP_LOG_WARN("No points to undo");
    }
}
