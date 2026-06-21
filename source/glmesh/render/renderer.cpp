/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: renderer.cpp
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
#include "renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

GLMESH_NAMESPACE_BEGIN

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

// ========== Viewport 管理 ==========

void Renderer::setViewport(float x_norm, float y_norm, float width_norm, float height_norm)
{
    viewport_x_norm_ = std::clamp(x_norm, 0.0f, 1.0f);
    viewport_y_norm_ = std::clamp(y_norm, 0.0f, 1.0f);
    viewport_width_norm_ = std::clamp(width_norm, 0.0f, 1.0f);
    viewport_height_norm_ = std::clamp(height_norm, 0.0f, 1.0f);
}

void Renderer::setViewportInPixels(int x, int y, int width, int height)
{
    if(render_window_width_ > 0 && render_window_height_ > 0){
        viewport_x_norm_ = static_cast<float>(x) / render_window_width_;
        viewport_y_norm_ = static_cast<float>(y) / render_window_height_;
        viewport_width_norm_ = static_cast<float>(width) / render_window_width_;
        viewport_height_norm_ = static_cast<float>(height) / render_window_height_;
    }
}

void Renderer::getViewport(float& x_norm, float& y_norm, float& width_norm, float& height_norm) const
{
    x_norm = viewport_x_norm_;
    y_norm = viewport_y_norm_;
    width_norm = viewport_width_norm_;
    height_norm = viewport_height_norm_;
}

void Renderer::getViewportInPixels(int& x, int& y, int& width, int& height) const
{
    x = static_cast<int>(viewport_x_norm_ * render_window_width_);
    y = static_cast<int>(viewport_y_norm_ * render_window_height_);
    width = static_cast<int>(viewport_width_norm_ * render_window_width_);
    height = static_cast<int>(viewport_height_norm_ * render_window_height_);
}

void Renderer::setRenderWindowSize(int width, int height)
{
    render_window_width_ = std::max(1, width);
    render_window_height_ = std::max(1, height);
}

void Renderer::getRenderWindowSize(int& width, int& height) const
{
    width = render_window_width_;
    height = render_window_height_;
}

// ========== 背景管理 ==========

void Renderer::setBackground(const glm::vec3& color)
{
    background_color_ = color;
    use_gradient_background_ = false;
}

void Renderer::setBackgroundGradient(const glm::vec3& color_top, const glm::vec3& color_bottom)
{
    background_color_top_ = color_top;
    background_color_bottom_ = color_bottom;
    use_gradient_background_ = true;
}

// ========== Camera 管理 ==========

void Renderer::setCamera(Camera* camera)
{
    camera_ = camera;
}

void Renderer::resetCamera()
{
    if(camera_){
        camera_->reset();
    }
}

void Renderer::resetCamera(const glm::vec3& bounds_min, const glm::vec3& bounds_max)
{
    if(camera_){
        camera_->fitBounds(bounds_min, bounds_max);
    }
}

// ========== 坐标系转换 ==========

glm::vec3 Renderer::worldToDisplay(const glm::vec3& world_pos) const
{
    if(!camera_){
        return glm::vec3(0.0f);
    }

    // 世界坐标 → 裁剪空间
    glm::mat4 view_proj = camera_->projectionMatrix() * camera_->viewMatrix();
    glm::vec4 clip_pos = view_proj * glm::vec4(world_pos, 1.0f);

    // 透视除法 → NDC（标准化设备坐标）
    if(std::abs(clip_pos.w) < 1e-6f){
        return glm::vec3(0.0f);
    }
    glm::vec3 ndc = glm::vec3(clip_pos) / clip_pos.w;

    // NDC → Viewport 像素坐标
    int vp_x, vp_y, vp_width, vp_height;
    getViewportInPixels(vp_x, vp_y, vp_width, vp_height);

    float display_x = vp_x + (ndc.x * 0.5f + 0.5f) * vp_width;
    float display_y = vp_y + (ndc.y * 0.5f + 0.5f) * vp_height;
    float depth = ndc.z * 0.5f + 0.5f; // [0, 1]

    return glm::vec3(display_x, display_y, depth);
}

glm::vec3 Renderer::displayToWorld(const glm::vec3& display_pos) const
{
    if(!camera_){
        return glm::vec3(0.0f);
    }

    // Viewport 像素坐标 → NDC
    int vp_x, vp_y, vp_width, vp_height;
    getViewportInPixels(vp_x, vp_y, vp_width, vp_height);

    float ndc_x = (display_pos.x - vp_x) / vp_width * 2.0f - 1.0f;
    float ndc_y = (display_pos.y - vp_y) / vp_height * 2.0f - 1.0f;
    float ndc_z = display_pos.z * 2.0f - 1.0f;

    // NDC → 裁剪空间 → 世界空间
    glm::mat4 inv_view_proj = glm::inverse(camera_->projectionMatrix() * camera_->viewMatrix());
    glm::vec4 clip_pos(ndc_x, ndc_y, ndc_z, 1.0f);
    glm::vec4 world_pos = inv_view_proj * clip_pos;

    if(std::abs(world_pos.w) < 1e-6f){
        return glm::vec3(0.0f);
    }

    return glm::vec3(world_pos) / world_pos.w;
}

glm::vec3 Renderer::worldToView(const glm::vec3& world_pos) const
{
    if(!camera_){
        return glm::vec3(0.0f);
    }

    glm::vec4 view_pos = camera_->viewMatrix() * glm::vec4(world_pos, 1.0f);
    return glm::vec3(view_pos);
}

glm::vec3 Renderer::viewToWorld(const glm::vec3& view_pos) const
{
    if(!camera_){
        return glm::vec3(0.0f);
    }

    glm::mat4 inv_view = glm::inverse(camera_->viewMatrix());
    glm::vec4 world_pos = inv_view * glm::vec4(view_pos, 1.0f);
    return glm::vec3(world_pos);
}

glm::vec3 Renderer::displayToView(const glm::vec3& display_pos) const
{
    // Display → World → View
    glm::vec3 world_pos = displayToWorld(display_pos);
    return worldToView(world_pos);
}

glm::vec3 Renderer::viewToDisplay(const glm::vec3& view_pos) const
{
    // View → World → Display
    glm::vec3 world_pos = viewToWorld(view_pos);
    return worldToDisplay(world_pos);
}

glm::vec3 Renderer::convertCoordinate(const glm::vec3& pos,
                                      CoordinateSystem from_system,
                                      CoordinateSystem to_system) const
{
    if(from_system == to_system){
        return pos;
    }

    // 所有转换路径通过 World 作为中间系统
    glm::vec3 world_pos = pos;

    // From → World
    switch (from_system){
        case CoordinateSystem::World:
            world_pos = pos;
            break;
        case CoordinateSystem::View:
            world_pos = viewToWorld(pos);
            break;
        case CoordinateSystem::Display:
            world_pos = displayToWorld(pos);
            break;
    }

    // World → To
    switch (to_system){
        case CoordinateSystem::World:
            return world_pos;
        case CoordinateSystem::View:
            return worldToView(world_pos);
        case CoordinateSystem::Display:
            return worldToDisplay(world_pos);
    }

    return world_pos;
}

// ========== 射线投射 ==========

Ray Renderer::computePickRay(float screen_x, float screen_y) const
{
    if(!camera_){
        return Ray{glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f)};
    }

    int vp_x, vp_y, vp_width, vp_height;
    getViewportInPixels(vp_x, vp_y, vp_width, vp_height);

    // 使用 near 和 far 平面的两个点生成射线
    glm::vec3 near_point = displayToWorld(glm::vec3(screen_x, screen_y, 0.0f));
    glm::vec3 far_point = displayToWorld(glm::vec3(screen_x, screen_y, 1.0f));

    Ray ray;
    ray.origin = near_point;
    ray.direction = glm::normalize(far_point - near_point);
    return ray;
}

glm::vec3 Renderer::unprojectWithDepth(float screen_x, float screen_y, float depth, const glm::mat4& model_matrix) const
{
    if(!camera_){
        return glm::vec3(0.0f);
    }

    // 获取 Viewport 尺寸
    int vp_x, vp_y, vp_width, vp_height;
    getViewportInPixels(vp_x, vp_y, vp_width, vp_height);

    // 屏幕坐标 → NDC（Qt 风格：左上角为原点，需要翻转 Y 轴）
    float ndc_x = (2.0f * screen_x) / vp_width - 1.0f;
    float ndc_y = 1.0f - (2.0f * screen_y) / vp_height; // 翻转 Y 轴
    float ndc_z = 2.0f * depth - 1.0f;                  // [0,1] → [-1,1]

    glm::vec4 clip_pos(ndc_x, ndc_y, ndc_z, 1.0f);

    // 构建 MVP 矩阵并求逆
    glm::mat4 mvp = camera_->projectionMatrix() * camera_->viewMatrix() * model_matrix;
    glm::mat4 inv_mvp = glm::inverse(mvp);

    // 逆变换：裁剪空间 → 世界空间
    glm::vec4 world_pos = inv_mvp * clip_pos;

    // 透视除法
    if(std::abs(world_pos.w) > 1e-6f){
        world_pos /= world_pos.w;
    }

    return glm::vec3(world_pos);
}

std::optional<glm::vec3> Renderer::pickWorldPoint(float screen_x, float screen_y, float depth,
                                                   const glm::mat4& model_matrix) const
{
    // 深度值接近 1.0 表示远裁剪面，即没有拾取到几何体
    if(depth >= 0.9999f){
        return std::nullopt;
    }
    return unprojectWithDepth(screen_x, screen_y, depth, model_matrix);
}

// ========== 渲染层级管理 ==========

void Renderer::setLayer(int layer)
{
    layer_ = layer;
}

void Renderer::setTransparent(bool transparent)
{
    transparent_ = transparent;
}

// ========== 渲染状态 ==========

void Renderer::clear()
{
    // OpenGL 调用应该由使用 Renderer 的代码来执行
    // 这里仅作为接口占位，实际的 glClear 需要在 OpenGL 上下文中调用
}

void Renderer::applyViewport()
{
    // OpenGL 调用应该由使用 Renderer 的代码来执行
    // 使用者可以调用 getViewportInPixels() 获取参数后再调用 glViewport
}

void Renderer::applyBackground()
{
    // OpenGL 调用应该由使用 Renderer 的代码来执行
    // 使用者可以通过 background() 等方法获取背景色后再调用 glClearColor
}

void Renderer::setDepthTestEnabled(bool enabled)
{
    depth_test_enabled_ = enabled;
    // OpenGL 调用应该由使用 Renderer 的代码来执行
    // 使用者可以根据 isDepthTestEnabled() 的返回值来调用 glEnable/glDisable(GL_DEPTH_TEST)
}

// ========== 实用工具 ==========

bool Renderer::isInViewport(float screen_x, float screen_y) const
{
    int vp_x, vp_y, vp_width, vp_height;
    getViewportInPixels(vp_x, vp_y, vp_width, vp_height);

    return screen_x >= vp_x &&
           screen_x < vp_x + vp_width &&
           screen_y >= vp_y &&
           screen_y < vp_y + vp_height;
}

float Renderer::viewportAspectRatio() const
{
    int vp_x, vp_y, vp_width, vp_height;
    getViewportInPixels(vp_x, vp_y, vp_width, vp_height);

    if(vp_height > 0){
        return static_cast<float>(vp_width) / static_cast<float>(vp_height);
    }
    return 1.0f;
}

GLMESH_NAMESPACE_END

