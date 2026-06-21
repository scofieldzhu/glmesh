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
#include "glmesh/kernel/gl/gpu_polyline.h"
#include "mesh_widget.h"
#include "orbit_interaction.h"
#include "app_log.h"

void DrawPolylineInteraction::onMousePress(QMouseEvent* event, const MouseInteractionContext& ctx)
{
    if(event->button() == Qt::LeftButton){
        // 左键点击：读取深度缓冲，通过 Renderer 拾取世界坐标
        if(ctx.readDepthFunc && ctx.renderer && ctx.runInGLContext){
            float depth = ctx.readDepthFunc(event->x(), event->y());
            glm::mat4 model_matrix = ctx.camera ? ctx.camera->modelCenterMatrix() : glm::mat4(1.0f);
            auto world_pt_opt = ctx.renderer->pickWorldPoint(
                static_cast<float>(event->x()),
                static_cast<float>(event->y()),
                depth,
                model_matrix
            );
            if(world_pt_opt){
                polyline_points_.push_back(*world_pt_opt);
                // 更新临时折线预览（需要 GL 上下文）
                ctx.runInGLContext([&](){
                    auto preview_obj = createPolyline(polyline_points_, glm::vec3(1.0f, 1.0f, 0.0f));
                    ctx.scene_manager->setTempObject("polyline_preview", std::move(preview_obj));
                });
                ctx.request_update_func();
                APP_LOG_INFO("Polyline point {} picked: ({:.3f}, {:.3f}, {:.3f})",
                    polyline_points_.size(),
                    world_pt_opt->x, world_pt_opt->y, world_pt_opt->z);
            }else{
                APP_LOG_WARN("No geometry picked at screen position ({}, {})", event->x(), event->y());
            }
        }
    }else if(event->button() == Qt::RightButton){
        // 右键点击：完成绘制并退出模式
        if(polyline_points_.size() >= 2){
            ctx.runInGLContext([&](){
                ctx.scene_manager->clearTempObject("polyline_preview");
                auto final_obj = createPolyline(polyline_points_, glm::vec3(0.0f, 1.0f, 0.0f));
                ctx.scene_manager->addObject(std::move(final_obj));
            });
            APP_LOG_INFO("Polyline drawing finished with {} points",
                polyline_points_.size());
        }else{
            APP_LOG_WARN("Need at least 2 points to finish polyline, canceling");
            ctx.scene_manager->clearTempObject("polyline_preview");
        }
        polyline_points_.clear();
        ctx.request_update_func();

        // 恢复默认的 Orbit 交互模式
        ctx.mesh_widget_->setMouseInteraction(std::make_unique<OrbitInteraction>());
        APP_LOG_INFO("Exited polyline drawing mode");
    }else if(event->button() == Qt::MiddleButton){
        // 中键：撤销最后一个点
        undoLastPoint();
        if(!polyline_points_.empty()){
            ctx.runInGLContext([&](){
                auto preview_obj = createPolyline(polyline_points_, glm::vec3(1.0f, 1.0f, 0.0f));
                ctx.scene_manager->setTempObject("polyline_preview", std::move(preview_obj));
            });
            ctx.request_update_func();
        }else{
            ctx.scene_manager->clearTempObject("polyline_preview");
            ctx.request_update_func();
        }
    }
}

void DrawPolylineInteraction::onMouseMove(QMouseEvent* event, const MouseInteractionContext& ctx)
{
    // 实时预览：显示从最后一个点到鼠标位置的连线
    if(!preview_mode_enabled_ || polyline_points_.empty()){
        return;
    }

    // 读取深度缓冲，通过 Renderer 拾取世界坐标
    if(ctx.readDepthFunc && ctx.renderer && ctx.runInGLContext){
        float depth = ctx.readDepthFunc(event->x(), event->y());
        glm::mat4 model_matrix = ctx.camera ? ctx.camera->modelCenterMatrix() : glm::mat4(1.0f);
        auto world_pt_opt = ctx.renderer->pickWorldPoint(
            static_cast<float>(event->x()),
            static_cast<float>(event->y()),
            depth,
            model_matrix
        );
        if(world_pt_opt){
            // 构建预览折线：已有点 + 当前鼠标位置（需要 GL 上下文）
            std::vector<glm::vec3> preview_points = polyline_points_;
            preview_points.push_back(*world_pt_opt);
            ctx.runInGLContext([&](){
                auto preview_obj = createPolyline(preview_points, glm::vec3(1.0f, 1.0f, 0.0f));
                ctx.scene_manager->setTempObject("polyline_preview", std::move(preview_obj));
            });
            ctx.request_update_func();
        }
    }
}

void DrawPolylineInteraction::onWheel(QWheelEvent* event, const MouseInteractionContext& ctx)
{
    // 折线绘制模式下禁用滚轮缩放，避免误操作
    // 用户可以按 ESC 退出绘制模式后再缩放
}

void DrawPolylineInteraction::onKeyPress(QKeyEvent* event, const MouseInteractionContext& ctx)
{
    if(event->key() == Qt::Key_Escape){
        // ESC：取消绘制并退出
        APP_LOG_INFO("Polyline drawing canceled (ESC pressed)");
        cancelDrawing();
        ctx.scene_manager->clearTempObject("polyline_preview");
        ctx.request_update_func();

        // 恢复默认交互
        ctx.mesh_widget_->setMouseInteraction(std::make_unique<OrbitInteraction>());
    }else if(event->key() == Qt::Key_Z && (event->modifiers() & Qt::ControlModifier)){
        // Ctrl+Z：撤销最后一个点
        undoLastPoint();
        if(!polyline_points_.empty()){
            ctx.runInGLContext([&](){
                auto preview_obj = createPolyline(polyline_points_, glm::vec3(1.0f, 1.0f, 0.0f));
                ctx.scene_manager->setTempObject("polyline_preview", std::move(preview_obj));
            });
            ctx.request_update_func();
        }else{
            ctx.scene_manager->clearTempObject("polyline_preview");
            ctx.request_update_func();
        }
    }else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        // Enter：完成绘制
        if(polyline_points_.size() >= 2){
            ctx.runInGLContext([&](){
                ctx.scene_manager->clearTempObject("polyline_preview");
                auto final_obj = createPolyline(polyline_points_, glm::vec3(0.0f, 1.0f, 0.0f));
                ctx.scene_manager->addObject(std::move(final_obj));
            });
            APP_LOG_INFO("Polyline drawing finished with {} points (Enter pressed)",
                polyline_points_.size());
            polyline_points_.clear();
            ctx.request_update_func();
        }else{
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
    if(!polyline_points_.empty()){
        polyline_points_.pop_back();
        APP_LOG_INFO("Undo last point, {} points remaining", polyline_points_.size());
    }else{
        APP_LOG_WARN("No points to undo");
    }
}

RenderableObject DrawPolylineInteraction::createPolyline(
    const std::vector<glm::vec3>& points,
    const glm::vec3& color,
    float line_width)
{
    if(points.size() < 2){
        APP_LOG_ERROR("Cannot create polyline with less than 2 points");
        return RenderableObject{};
    }

    // 创建 GPU 数据
    glmesh::GpuPolyline<glmesh::GpuVertexPC> gpu_polyline;
    gpu_polyline.vertexes.reserve(points.size());

    for(const auto& pt : points){
        glmesh::GpuVertexPC vertex;
        vertex.position = pt;
        vertex.color = color;
        gpu_polyline.vertexes.push_back(vertex);
    }

    // 创建索引（连续的线段）
    gpu_polyline.indexes.reserve((points.size() - 1) * 2);
    for(size_t i = 0; i < points.size() - 1; ++i){
        gpu_polyline.indexes.push_back(static_cast<glmesh::int32>(i));
        gpu_polyline.indexes.push_back(static_cast<glmesh::int32>(i + 1));
    }

    // 创建 GLPolyline 并上传
    auto gl_polyline = std::make_shared<glmesh::GLPolyline>();
    gl_polyline->upload(gpu_polyline, 0x88E4);  // 0x88E4 = GL_STATIC_DRAW

    // 创建 RenderableObject
    RenderableObject renderable;
    renderable.visible = true;
    renderable.drawable = gl_polyline;
    renderable.model_matrix = glm::mat4(1.0f);
    renderable.material.shader_prog_id = SPT_MESH;
    renderable.material.render_mode = MeshRenderMode::Wireframe;
    renderable.material.line_width = line_width;
    renderable.material.base_color = color;
    renderable.material.use_vertex_color = true;
    return renderable;
}
