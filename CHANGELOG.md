# Changelog

## v1.0.0 (2026-05-17)

### 项目架构

- 基于 CMake + vcpkg 构建系统，支持 MSVC 2022
- 三层模块化架构：`glmesh.kernel`（核心库）、`glmesh.render`（渲染库）、`MeshStudio`（桌面应用）
- 引入 GLAD 作为 OpenGL 3.0+ 函数加载器，GLM 作为数学库，spdlog 作为日志库
- 依赖 Qt5（Core/Widgets/OpenGL）、PCL（点云库）进行 UI 与模型 I/O

### 核心库 (glmesh.kernel)

- **数据结构**：定义 CpuVertex、CpuTriangleMesh、CpuPointCloud、CpuBkg、Bounds3D 等 CPU 侧数据类型
- **几何工具**：提供包围盒计算、多边形三角化等几何处理接口
- **OpenGL 封装**：实现 VertexArray、VertexBuffer、IndexBuffer、ShaderProgram 等 GPU 资源管理类
- **可绘制对象**：支持 GLTriangleMesh（三角网格）、GLPointCloud（点云）、GLRectangle（矩形）、GLBkg（渐变背景）渲染
- **Trackball Gizmo**：新增坐标轴指示器组件，支持三轴高亮交互与鼠标拾取
- **I/O**：支持 PLY 格式文件的点云与面片数据加载
- **日志**：集成 spdlog，提供统一的日志输出模块

### 渲染库 (glmesh.render)

- **Camera**：实现轨道相机（orbit/pan/zoom），支持透视与正交投影，基于四元数的旋转控制

### MeshStudio 桌面应用

- **主界面**：基于 QMainWindow 的多面板布局，包含模型树、属性面板、显示模式控制
- **MeshWidget**：基于 QOpenGLWidget 的 3D 渲染窗口，集成相机交互与渲染管线
- **ArcBallRotator**：封装 arcball 旋转数学逻辑，支持鼠标拖拽旋转模型
- **ShaderProgramManager**：单例模式管理着色器程序生命周期，支持背景、模型、Gizmo 三类着色器
- **ColorButton**：颜色编辑控件，用于环境光/漫反射光颜色配置
- **材质与光照**：
  - 环境光颜色及强度控制
  - 漫反射光颜色、强度及光源方向控制
  - 高光反射计算
- **显示模式**：支持点云与三角网格两种渲染模式切换
- **背景**：基于 NDC 坐标系的渐变背景渲染

### 技术细节

- C++20 标准，使用 `GLM_ENABLE_EXPERIMENTAL` 扩展
- 命名空间 `glmesh`，通过 `GLMESH_NAMESPACE_BEGIN`/`GLMESH_NAMESPACE_END` 宏管理
- DLL 导出通过 `GLMESH_KERNEL_API`/`GLMESH_RENDER_API` 控制
- 着色器以内联字符串形式编译，无需外部着色器文件
