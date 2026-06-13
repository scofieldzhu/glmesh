# Renderer 模块说明

## 概述

新增的 `Renderer` 类是一个参考 VTK 和 OSG 等成熟 3D 渲染库设计的渲染管理器。它提供了完整的视口管理、坐标系转换、多层渲染等功能。

## 设计理念

### 1. 参考成熟库的设计

- **VTK (Visualization Toolkit)** 风格
  - 标准化坐标 [0,1] 的 Viewport 管理
  - `displayToWorld` / `worldToDisplay` 坐标转换接口
  - 多层渲染支持（Layer）
  
- **OSG (OpenSceneGraph)** 风格
  - 射线投射（Ray Casting）用于拾取
  - Camera 集成
  - 灵活的视口配置

### 2. 关注点分离

`Renderer` 类**不直接调用 OpenGL 函数**，原因是：
- GLAD 符号在 `glmesh.kernel` DLL 中，`glmesh.render` 无法直接访问
- 保持渲染逻辑与 OpenGL 实现分离
- 使用者（如 `MeshWidget`）负责实际的 OpenGL 调用

这种设计使得 `Renderer` 作为一个"状态管理器"和"坐标转换器"，而不是 OpenGL 包装器。

## 核心功能

### 1. Viewport 管理

支持两种坐标系统：

```cpp
// 标准化坐标 [0,1]（VTK 风格）
renderer.setViewport(0.0f, 0.0f, 1.0f, 1.0f);

// 像素坐标
renderer.setViewportInPixels(0, 0, 800, 600);
```

### 2. 坐标系转换

三个坐标系：
- **World**：世界坐标系（3D 场景全局坐标）
- **View**：视图坐标系（相机空间）
- **Display**：显示坐标系（屏幕像素）

提供双向转换：
```cpp
glm::vec3 worldToDisplay(const glm::vec3& world_pos);
glm::vec3 displayToWorld(const glm::vec3& display_pos);
glm::vec3 worldToView(const glm::vec3& world_pos);
glm::vec3 viewToWorld(const glm::vec3& view_pos);
```

通用转换接口：
```cpp
glm::vec3 convertCoordinate(
    const glm::vec3& pos,
    CoordinateSystem from_system,
    CoordinateSystem to_system
);
```

### 3. 射线投射

用于 3D 拾取（Picking）：
```cpp
Ray computePickRay(float screen_x, float screen_y);
```

返回的 `Ray` 包含：
- `origin`：射线起点（世界坐标）
- `direction`：射线方向（归一化）

### 4. 多层渲染

支持多个 `Renderer` 叠加渲染：
```cpp
renderer1.setLayer(0);  // 背景层
renderer2.setLayer(1);  // 主场景层
renderer3.setLayer(2);  // UI 层
renderer3.setTransparent(true);  // 透明层不清除背景
```

### 5. 背景管理

```cpp
// 单色背景
renderer.setBackground(glm::vec3(0.2f, 0.2f, 0.2f));

// 渐变背景
renderer.setBackgroundGradient(
    glm::vec3(0.3f, 0.3f, 0.4f),  // 顶部
    glm::vec3(0.1f, 0.1f, 0.15f)  // 底部
);
```

## 文件结构

```
source/glmesh/render/
├── renderer.h                          # Renderer 类头文件
├── renderer.cpp                        # Renderer 类实现
├── renderer_usage.md                   # 详细使用指南
├── renderer_integration_example.cpp    # 集成示例代码
└── README_RENDERER.md                  # 本文件
```

## 使用场景

### 场景 1：单视图 3D 查看器

最常见的使用场景，一个全屏的 3D 视图：

```cpp
renderer_.setRenderWindowSize(width(), height());
renderer_.setViewport(0.0f, 0.0f, 1.0f, 1.0f);
renderer_.setCamera(&camera_);
```

### 场景 2：多视图布局（四视图）

类似 3ds Max / Blender 的四视图布局：

```cpp
// 四个 Renderer，每个占 1/4 屏幕
perspective_renderer_.setViewportInPixels(0, h/2, w/2, h/2);
top_renderer_.setViewportInPixels(w/2, h/2, w/2, h/2);
front_renderer_.setViewportInPixels(0, 0, w/2, h/2);
right_renderer_.setViewportInPixels(w/2, 0, w/2, h/2);
```

### 场景 3：主视图 + 缩略图

主视图占大部分区域，右上角有一个小的缩略图：

```cpp
main_renderer_.setViewport(0.0f, 0.0f, 1.0f, 1.0f);
thumbnail_renderer_.setViewport(0.7f, 0.7f, 0.28f, 0.28f);
```

### 场景 4：多层渲染（背景 + 场景 + UI）

```cpp
bg_renderer_.setLayer(0);      // 背景层
scene_renderer_.setLayer(1);   // 场景层
ui_renderer_.setLayer(2);      // UI 层（透明）
ui_renderer_.setTransparent(true);
```

## 与现有代码集成

### 在 MeshWidget 中使用

现有的 `MeshWidget` 可以逐步集成 `Renderer`：

1. **第一步：添加成员变量**
   ```cpp
   Renderer renderer_;
   ```

2. **第二步：初始化**
   ```cpp
   void MeshWidget::initializeGL() {
       renderer_.setRenderWindowSize(width(), height());
       renderer_.setCamera(&camera_);
   }
   ```

3. **第三步：使用坐标转换**
   ```cpp
   void MeshWidget::mousePressEvent(QMouseEvent* event) {
       Ray ray = renderer_.computePickRay(event->x(), event->y());
       // 使用 ray 进行拾取...
   }
   ```

### 不需要修改的部分

- 现有的 Camera 类完全兼容
- 现有的 GLDrawable 对象不需要修改
- 现有的 ShaderProgram 管理不需要修改

## 未来扩展

### 可能的功能扩展

1. **内置拾取器**
   ```cpp
   std::optional<PickResult> Renderer::pick(float x, float y, 
                                            const std::vector<GLDrawable*>& objects);
   ```

2. **渲染统计**
   ```cpp
   struct RenderStats {
       int triangles_rendered;
       int draw_calls;
       float render_time_ms;
   };
   ```

3. **后处理效果支持**
   ```cpp
   renderer.addPostEffect(std::make_unique<BloomEffect>());
   renderer.addPostEffect(std::make_unique<AAEffect>());
   ```

4. **帧缓冲对象集成**
   ```cpp
   renderer.setRenderTarget(framebuffer);
   ```

## API 对比

### VTK vs glmesh::Renderer

| VTK | glmesh::Renderer |
|-----|------------------|
| `vtkRenderer::SetViewport(x, y, w, h)` | `setViewport(x, y, w, h)` |
| `vtkRenderer::SetBackground(r, g, b)` | `setBackground(glm::vec3(r, g, b))` |
| `vtkRenderer::SetLayer(layer)` | `setLayer(layer)` |
| `vtkRenderer::GetActiveCamera()` | `camera()` |
| `vtkRenderer::ResetCamera()` | `resetCamera()` |
| `vtkCoordinate::SetCoordinateSystemToWorld()` | `CoordinateSystem::World` |
| `vtkCoordinate::GetComputedDisplayValue()` | `worldToDisplay()` |

### OSG vs glmesh::Renderer

| OSG | glmesh::Renderer |
|-----|------------------|
| `osg::Camera::setViewport()` | `setViewport()` |
| `osg::Camera::setClearColor()` | `setBackground()` |
| `osg::Camera::setRenderOrder()` | `setLayer()` |
| 射线投射通过 `computeIntersections` | `computePickRay()` |

## 技术细节

### 坐标转换管线

```
World Space (世界空间)
    │
    │ view_matrix
    ▼
View Space (视图空间/相机空间)
    │
    │ projection_matrix
    ▼
Clip Space (裁剪空间)
    │
    │ perspective_divide (w 分量除法)
    ▼
NDC (标准化设备坐标，[-1,1]³)
    │
    │ viewport_transform
    ▼
Display Space (显示空间，屏幕像素)
```

### 矩阵变换

```cpp
// World → Display
clip_pos = projection_matrix * view_matrix * world_pos
ndc = clip_pos / clip_pos.w
screen_pos.xy = (ndc.xy * 0.5 + 0.5) * viewport_size + viewport_origin

// Display → World
ndc = (screen_pos - viewport_origin) / viewport_size * 2.0 - 1.0
clip_pos = vec4(ndc, 1.0)
world_pos = inverse(projection_matrix * view_matrix) * clip_pos
world_pos /= world_pos.w
```

## 常见问题

### Q: 为什么 Renderer 不直接调用 OpenGL？

A: 因为 GLAD 符号在 `glmesh.kernel` DLL 中，`glmesh.render` 作为独立的 DLL 无法直接链接这些符号。这种设计也更符合关注点分离原则。

### Q: 如何应用 Viewport？

A: 使用 `getViewportInPixels()` 获取像素坐标后调用 `glViewport()`：
```cpp
int x, y, w, h;
renderer.getViewportInPixels(x, y, w, h);
glViewport(x, y, w, h);
```

### Q: 渐变背景如何实现？

A: 需要渲染一个全屏四边形（参考 `GLBkg` 类）：
```cpp
if (renderer.isGradientBackground()) {
    // 使用 GLBkg 渲染渐变背景
}
```

### Q: 能否多个 Renderer 共享一个 Camera？

A: 可以，但通常每个视图需要独立的 Camera（不同的视角）。

## 参考资料

- **VTK Documentation**: https://vtk.org/doc/nightly/html/classvtkRenderer.html
- **OSG Documentation**: http://www.openscenegraph.org/documentation/
- **OpenGL Viewport**: https://www.khronos.org/opengl/wiki/Viewport_Transform
- **详细使用指南**: `renderer_usage.md`
- **集成示例**: `renderer_integration_example.cpp`
