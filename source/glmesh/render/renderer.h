/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: renderer.h
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
#ifndef __renderer_h__
#define __renderer_h__

#include "glmesh/render/glmesh_render_typedef.h"
#include "glmesh/render/camera.h"
#include <memory>
#include <vector>
#include <functional>

GLMESH_NAMESPACE_BEGIN

/**
 * @brief Renderer 类：参考 VTK vtkRenderer 和 OSG osg::Camera 设计的渲染器
 *
 * 主要职责：
 * - Viewport 管理（支持标准化坐标 [0,1] 和像素坐标）
 * - 背景颜色/渐变管理
 * - Camera 集成
 * - 坐标系转换（World/View/Display 空间）
 * - 渲染状态管理
 * - 多 Renderer 层级支持
 */
class GLMESH_RENDER_API Renderer
{
public:
    // ========== 坐标系枚举 ==========
    enum class CoordinateSystem {
        World,      // 世界坐标系
        View,       // 视图坐标系（相机空间）
        Display     // 显示坐标系（屏幕像素空间，类似 VTK）
    };

    // ========== Viewport 管理 ==========

    /**
     * @brief 设置 Viewport（标准化坐标，范围 [0.0, 1.0]）
     * @param x_norm 左下角 X 坐标（标准化）
     * @param y_norm 左下角 Y 坐标（标准化）
     * @param width_norm 宽度（标准化）
     * @param height_norm 高度（标准化）
     *
     * 参考 VTK vtkViewport::SetViewport() 接口
     */
    void setViewport(float x_norm, float y_norm, float width_norm, float height_norm);

    /**
     * @brief 设置 Viewport（像素坐标）
     * @param x 左下角 X 像素坐标
     * @param y 左下角 Y 像素坐标
     * @param width 宽度（像素）
     * @param height 高度（像素）
     */
    void setViewportInPixels(int x, int y, int width, int height);

    /**
     * @brief 获取标准化 Viewport
     */
    void getViewport(float& x_norm, float& y_norm, float& width_norm, float& height_norm) const;

    /**
     * @brief 获取像素 Viewport（需要先设置 RenderWindow 尺寸）
     */
    void getViewportInPixels(int& x, int& y, int& width, int& height) const;

    /**
     * @brief 设置渲染窗口的实际尺寸（用于标准化坐标转像素坐标）
     */
    void setRenderWindowSize(int width, int height);

    /**
     * @brief 获取渲染窗口尺寸
     */
    void getRenderWindowSize(int& width, int& height) const;

    // ========== 背景管理 ==========

    /**
     * @brief 设置单色背景
     */
    void setBackground(const glm::vec3& color);

    /**
     * @brief 设置渐变背景（从上到下）
     */
    void setBackgroundGradient(const glm::vec3& color_top, const glm::vec3& color_bottom);

    /**
     * @brief 获取背景颜色
     */
    const glm::vec3& background() const { return background_color_; }

    /**
     * @brief 是否启用渐变背景
     */
    bool isGradientBackground() const { return use_gradient_background_; }

    /**
     * @brief 获取渐变背景的顶部颜色
     */
    const glm::vec3& backgroundTop() const { return background_color_top_; }

    /**
     * @brief 获取渐变背景的底部颜色
     */
    const glm::vec3& backgroundBottom() const { return background_color_bottom_; }

    // ========== Camera 管理 ==========

    /**
     * @brief 设置活动相机
     */
    void setCamera(Camera* camera);

    /**
     * @brief 获取活动相机
     */
    Camera* camera() const { return camera_; }

    /**
     * @brief 重置相机（参考 VTK vtkRenderer::ResetCamera）
     */
    void resetCamera();

    /**
     * @brief 重置相机到指定边界
     */
    void resetCamera(const glm::vec3& bounds_min, const glm::vec3& bounds_max);

    // ========== 坐标系转换（核心功能，类似 VTK）==========

    /**
     * @brief World → Display 转换（3D 世界坐标 → 2D 屏幕像素坐标）
     * @param world_pos 世界坐标
     * @return 显示坐标（x, y 为像素，z 为深度 [0,1]）
     *
     * 参考 VTK vtkCoordinate::GetComputedDisplayValue()
     */
    glm::vec3 worldToDisplay(const glm::vec3& world_pos) const;

    /**
     * @brief Display → World 转换（2D 屏幕像素坐标 → 3D 世界坐标）
     * @param display_pos 显示坐标（x, y 为像素，z 为深度 [0,1]）
     * @return 世界坐标
     *
     * 参考 VTK vtkCoordinate::GetComputedWorldValue()
     */
    glm::vec3 displayToWorld(const glm::vec3& display_pos) const;

    /**
     * @brief World → View 转换（世界坐标 → 相机视图坐标）
     */
    glm::vec3 worldToView(const glm::vec3& world_pos) const;

    /**
     * @brief View → World 转换（相机视图坐标 → 世界坐标）
     */
    glm::vec3 viewToWorld(const glm::vec3& view_pos) const;

    /**
     * @brief Display → View 转换（屏幕像素坐标 → 相机视图坐标）
     */
    glm::vec3 displayToView(const glm::vec3& display_pos) const;

    /**
     * @brief View → Display 转换（相机视图坐标 → 屏幕像素坐标）
     */
    glm::vec3 viewToDisplay(const glm::vec3& view_pos) const;

    /**
     * @brief 通用坐标转换接口（参考 VTK vtkCoordinate）
     * @param pos 输入坐标
     * @param from_system 源坐标系
     * @param to_system 目标坐标系
     * @return 转换后的坐标
     */
    glm::vec3 convertCoordinate(const glm::vec3& pos,
                                CoordinateSystem from_system,
                                CoordinateSystem to_system) const;

    // ========== 射线投射（Picking 支持）==========

    /**
     * @brief 从屏幕坐标生成拾取射线（参考 OSG 的射线投射）
     * @param screen_x 屏幕 X 坐标（像素）
     * @param screen_y 屏幕 Y 坐标（像素）
     * @return 世界空间中的射线
     */
    Ray computePickRay(float screen_x, float screen_y) const;

    /**
     * @brief 使用深度值进行反投影（屏幕坐标 + 深度 → 世界坐标）
     * @param screen_x 屏幕 X 坐标（像素，Qt 风格左上角为原点）
     * @param screen_y 屏幕 Y 坐标（像素，Qt 风格左上角为原点）
     * @param depth 深度值 [0.0 (Near), 1.0 (Far)]，通常从 glReadPixels(GL_DEPTH_COMPONENT) 读取
     * @param model_matrix 模型矩阵（默认为单位矩阵）
     * @return 世界坐标
     *
     * 用于拾取：结合 GPU 深度缓冲值，精确还原被点击物体的世界坐标
     */
    glm::vec3 unprojectWithDepth(float screen_x, float screen_y, float depth,
                                  const glm::mat4& model_matrix = glm::mat4(1.0f)) const;

    // ========== 渲染层级管理（多 Renderer 支持）==========

    /**
     * @brief 设置渲染层级（参考 VTK vtkRenderer::SetLayer）
     * 层级值越大，越晚渲染（叠加在上层）
     */
    void setLayer(int layer);

    /**
     * @brief 获取渲染层级
     */
    int layer() const { return layer_; }

    /**
     * @brief 设置是否透明渲染（用于多层叠加）
     */
    void setTransparent(bool transparent);

    /**
     * @brief 是否透明渲染
     */
    bool isTransparent() const { return transparent_; }

    // ========== 渲染状态 ==========

    /**
     * @brief 清除颜色缓冲和深度缓冲（参考 VTK vtkRenderer::Clear）
     */
    void clear();

    /**
     * @brief 应用 Viewport 到 OpenGL（调用 glViewport）
     */
    void applyViewport();

    /**
     * @brief 应用背景色（调用 glClearColor）
     */
    void applyBackground();

    /**
     * @brief 设置是否启用深度测试
     */
    void setDepthTestEnabled(bool enabled);

    /**
     * @brief 是否启用深度测试
     */
    bool isDepthTestEnabled() const { return depth_test_enabled_; }

    // ========== 实用工具 ==========

    /**
     * @brief 判断屏幕点是否在当前 Viewport 内
     */
    bool isInViewport(float screen_x, float screen_y) const;

    /**
     * @brief 获取 Viewport 的宽高比
     */
    float viewportAspectRatio() const;

    // ========== 构造/析构 ==========

    Renderer();
    ~Renderer();

private:
    // Viewport（标准化坐标 [0,1]）
    float viewport_x_norm_ = 0.0f;
    float viewport_y_norm_ = 0.0f;
    float viewport_width_norm_ = 1.0f;
    float viewport_height_norm_ = 1.0f;

    // 渲染窗口尺寸（像素）
    int render_window_width_ = 800;
    int render_window_height_ = 600;

    // 背景
    glm::vec3 background_color_ = {0.2f, 0.2f, 0.2f};
    bool use_gradient_background_ = false;
    glm::vec3 background_color_top_ = {0.3f, 0.3f, 0.4f};
    glm::vec3 background_color_bottom_ = {0.1f, 0.1f, 0.15f};

    // Camera
    Camera* camera_ = nullptr;

    // 渲染层级
    int layer_ = 0;
    bool transparent_ = false;

    // 渲染状态
    bool depth_test_enabled_ = true;
};

GLMESH_NAMESPACE_END

#endif