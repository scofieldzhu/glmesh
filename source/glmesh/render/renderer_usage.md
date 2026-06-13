# Renderer 类使用指南

## 概述

`Renderer` 类参考了 VTK 和 OSG 等成熟 3D 渲染库的设计理念，提供了一套完整的渲染管理接口。

## 核心功能

### 1. Viewport 管理

支持标准化坐标 [0,1] 和像素坐标两种方式：

```cpp
// 使用标准化坐标（类似 VTK）
renderer.setViewport(0.0f, 0.0f, 1.0f, 1.0f);  // 全屏

// 使用像素坐标
renderer.setRenderWindowSize(800, 600);
renderer.setViewportInPixels(0, 0, 400, 600);  // 左半屏

// 获取 Viewport 信息
float x, y, w, h;
renderer.getViewport(x, y, w, h);

// 应用 Viewport（需要在 OpenGL 上下文中）
int px, py, pw, ph;
renderer.getViewportInPixels(px, py, pw, ph);
glViewport(px, py, pw, ph);
```

### 2. 背景管理

```cpp
// 单色背景
renderer.setBackground(glm::vec3(0.2f, 0.2f, 0.2f));

// 渐变背景（从上到下）
renderer.setBackgroundGradient(
    glm::vec3(0.3f, 0.3f, 0.4f),  // 顶部颜色
    glm::vec3(0.1f, 0.1f, 0.15f)  // 底部颜色
);

// 应用背景（需要在 OpenGL 上下文中）
if (renderer.isGradientBackground()) {
    // 使用渐变背景需要渲染四边形
    // 可以参考 GLBkg 类的实现
} else {
    const auto& bg = renderer.background();
    glClearColor(bg.r, bg.g, bg.b, 1.0f);
}
```

### 3. Camera 集成

```cpp
Camera camera;
renderer.setCamera(&camera);

// 重置相机
renderer.resetCamera();

// 重置到指定边界
glm::vec3 bounds_min(-10.0f, -10.0f, -10.0f);
glm::vec3 bounds_max(10.0f, 10.0f, 10.0f);
renderer.resetCamera(bounds_min, bounds_max);
```

### 4. 坐标系转换（核心功能）

`Renderer` 提供了三个坐标系之间的转换：

- **World（世界坐标系）**：3D 场景中的全局坐标
- **View（视图坐标系）**：相机空间坐标
- **Display（显示坐标系）**：屏幕像素坐标

#### 4.1 World ↔ Display 转换

```cpp
// World → Display（3D 世界坐标 → 2D 屏幕像素）
glm::vec3 world_pos(1.0f, 2.0f, 3.0f);
glm::vec3 screen_pos = renderer.worldToDisplay(world_pos);
// screen_pos.xy 是像素坐标，screen_pos.z 是深度 [0,1]

// Display → World（2D 屏幕像素 → 3D 世界坐标）
glm::vec3 display_pos(400.0f, 300.0f, 0.5f);  // x, y 为像素，z 为深度
glm::vec3 world_pos = renderer.displayToWorld(display_pos);
```

#### 4.2 World ↔ View 转换

```cpp
// World → View（世界坐标 → 相机视图坐标）
glm::vec3 view_pos = renderer.worldToView(world_pos);

// View → World（相机视图坐标 → 世界坐标）
glm::vec3 world_pos = renderer.viewToWorld(view_pos);
```

#### 4.3 通用坐标转换接口

```cpp
using CS = Renderer::CoordinateSystem;

// 任意坐标系之间的转换
glm::vec3 result = renderer.convertCoordinate(
    pos,
    CS::World,    // 源坐标系
    CS::Display   // 目标坐标系
);
```

### 5. 射线投射（Picking 支持）

```cpp
// 从屏幕坐标生成拾取射线
Ray ray = renderer.computePickRay(mouse_x, mouse_y);

// 使用射线进行拾取
// ray.origin 是射线起点（世界坐标）
// ray.direction 是射线方向（归一化）

// 射线与平面/物体求交
float t;
if (intersectRayWithPlane(ray, plane, t)) {
    glm::vec3 hit_point = ray.origin + ray.direction * t;
}
```

### 6. 多 Renderer 层级支持

支持多个 Renderer 叠加渲染（类似 VTK 的多层渲染）：

```cpp
// 创建多个 Renderer
Renderer bg_renderer;    // 背景层
Renderer main_renderer;  // 主渲染层
Renderer ui_renderer;    // UI 覆盖层

// 设置层级（数值越大越晚渲染，叠加在上层）
bg_renderer.setLayer(0);
main_renderer.setLayer(1);
ui_renderer.setLayer(2);

// UI 层设置为透明，不清除背景
ui_renderer.setTransparent(true);

// 渲染循环
for (auto* renderer : {&bg_renderer, &main_renderer, &ui_renderer}) {
    int x, y, w, h;
    renderer->getViewportInPixels(x, y, w, h);
    glViewport(x, y, w, h);
    
    if (!renderer->isTransparent()) {
        const auto& bg = renderer->background();
        glClearColor(bg.r, bg.g, bg.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    // 渲染该层的内容...
}
```

### 7. 实用工具

```cpp
// 判断屏幕点是否在 Viewport 内
if (renderer.isInViewport(mouse_x, mouse_y)) {
    // 处理鼠标事件
}

// 获取 Viewport 宽高比
float aspect = renderer.viewportAspectRatio();
```

## 完整示例

### 在 QOpenGLWidget 中使用

```cpp
class MyGLWidget : public QOpenGLWidget
{
public:
    void initializeGL() override
    {
        // 初始化 OpenGL
        gladLoadGLLoader((GLADloadproc)QOpenGLContext::currentContext()->getProcAddress);
        
        // 设置 Renderer
        renderer_.setRenderWindowSize(width(), height());
        renderer_.setBackground(glm::vec3(0.2f, 0.2f, 0.2f));
        
        // 设置 Camera
        camera_.setViewport(width(), height());
        camera_.setPerspective(45.0f, 0.1f, 1000.0f);
        renderer_.setCamera(&camera_);
    }
    
    void resizeGL(int w, int h) override
    {
        renderer_.setRenderWindowSize(w, h);
        camera_.setViewport(w, h);
    }
    
    void paintGL() override
    {
        // 应用 Viewport
        int x, y, w, h;
        renderer_.getViewportInPixels(x, y, w, h);
        glViewport(x, y, w, h);
        
        // 应用背景并清屏
        const auto& bg = renderer_.background();
        glClearColor(bg.r, bg.g, bg.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 渲染场景...
    }
    
    void mousePressEvent(QMouseEvent* event) override
    {
        // 使用射线投射进行拾取
        Ray ray = renderer_.computePickRay(event->x(), event->y());
        
        // 执行拾取逻辑...
    }

private:
    Renderer renderer_;
    Camera camera_;
};
```

## VTK/OSG 设计对比

| 功能 | VTK | OSG | glmesh::Renderer |
|------|-----|-----|------------------|
| Viewport 标准化坐标 | ✓ vtkViewport::SetViewport | ✓ osg::Camera::setViewport | ✓ setViewport |
| 坐标系转换 | ✓ vtkCoordinate | ✓ computeLocalToWorld | ✓ convertCoordinate |
| 多层渲染 | ✓ vtkRenderer::SetLayer | ✓ osg::Camera::RenderOrder | ✓ setLayer |
| 射线投射 | ✓ vtkWorldPointPicker | ✓ computeIntersections | ✓ computePickRay |
| Camera 集成 | ✓ vtkCamera | ✓ osg::Camera | ✓ Camera* |

## 设计理念

1. **关注点分离**：Renderer 负责坐标转换和视口管理，不直接调用 OpenGL 函数
2. **灵活性**：支持多种坐标系统和多层渲染
3. **VTK 风格接口**：标准化坐标 [0,1]、displayToWorld 等接口名称与 VTK 一致
4. **OSG 风格特性**：射线投射、层级管理借鉴 OSG 设计
