# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Quick Start

Requires: Visual Studio 2022, Qt5 (msvc2017_64), vcpkg, CMake 3.15+.

**Setup prerequisite:** Before configuring, ensure `CMakeUserPresets.json` exists with `CMAKE_TOOLCHAIN_FILE` pointing to the vcpkg toolchain and `CMAKE_PREFIX_PATH` pointing to the Qt5 installation directory.

**Common commands:**
```bash
cmake --preset vs2022-local                    # Configure
cmake --build --preset build-debug-local       # Build Debug
cmake --build --preset build-release-local     # Build Release
./build/vs2022-local/bin/MeshStudio.exe        # Run the application
```

## Build Commands

This project uses CMake with vcpkg for dependency management. The base preset (`vs2022`) is defined in `CMakePresets.json` at the repo root. The local preset (`vs2022-local`) requires a `CMakeUserPresets.json` that sets `CMAKE_TOOLCHAIN_FILE` to the vcpkg toolchain and `CMAKE_PREFIX_PATH` to the Qt5 installation.

```bash
# Configure
cmake --preset vs2022-local

# Build Debug
cmake --build --preset build-debug-local

# Build Release
cmake --build --preset build-release-local

# Install (optional; copies binaries, DLLs, and headers to CMAKE_INSTALL_PREFIX, default: ./install/)
# Run windeployqt to collect Qt5 runtime DLLs alongside vcpkg dependency DLLs
cmake --install build/vs2022-local --config Debug
cmake --install build/vs2022-local --config Release
```

Output binaries go to `build/vs2022-local/bin/`. The install step is useful for packaging/distribution but not required during development.

## Dependencies

- **Qt5** (Core, Widgets, OpenGL) - UI framework and OpenGL widget hosting
- **PCL** (Point Cloud Library) - mesh/point cloud I/O (PLY loading)
- **GLM** - math (vectors, matrices, quaternions); used with `GLM_ENABLE_EXPERIMENTAL`
- **spdlog** - logging (trace level active via `SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE`)
- **GLAD** - OpenGL 3.0+ function loader (bundled in `source/glmesh/kernel/third_party/glad/`)

All external deps except GLAD are resolved via vcpkg. Qt5 is found via `CMAKE_PREFIX_PATH`.

## Architecture

> **Note:** `glmesh.md` at the repo root is an older, outdated reference. CLAUDE.md is the canonical architecture document.

The project is a C++20 OpenGL mesh viewer with three modules under `source/glmesh/`:

### kernel (`glmesh.kernel` shared library)
Low-level OpenGL abstractions and data types. Three sub-layers:
- **core/** - CPU-side data structures: `CpuVertex`, `CpuTriangleMesh`, `CpuPointCloud`, `CpuBkg`, `Bounds3D`, geometry utilities
- **gl/** - GPU-side OpenGL wrappers: `VertexArray`, `VertexBuffer`, `IndexBuffer`, `ShaderProgram`, and drawable types (`GLTriangleMesh`, `GLPointCloud`, `GLRectangle`, `GLBkg`, `GLTrackballGizmo`, `GLPolyline`) implementing the `GLDrawable` interface
- **io/** - mesh file loading (`LoadPlyAsCpuPolygonMesh`)
- **cpu_to_gpu** - conversion functions from CPU structs to GPU structs (e.g. `ToGpuTriangleMesh`, `ToGpuVertex`)

### render (`glmesh.render` shared library)
Higher-level rendering utilities:
- **Camera** - orbit/pan/zoom, perspective/orthographic projection, quaternion-based rotation. Focuses on view/projection matrix generation and camera operations. Viewport management is handled by Renderer.
- **Renderer** - viewport management, background control, coordinate system conversions (World/View/Display), multi-layer rendering support. Inspired by VTK's vtkRenderer and OSG's osg::Camera design patterns. Handles all coordinate transformations including screen-to-world unprojection with depth values.

### mesh_studio (`MeshStudio` executable)
Qt5 application that ties everything together:
- **MainWidget** - QMainWindow with UI for importing meshes, tree view, display mode controls, lighting parameters, and the polyline drawing action
- **MeshWidget** - QOpenGLWidget that manages the GL context, camera interaction, background gradient, and renders `RenderableObject` instances. Shader source strings are defined here as inline `const char*` literals.
- **SceneManager** - manages all renderable objects in the scene, separating object lifecycle from the widget. Supports permanent objects (meshes, finalized polylines) and temporary objects (drawing previews). Thread-safe via `std::mutex`. Provides generic `setTempObject`/`clearTempObject`/`finalizeTempObject` for managing preview objects. Does NOT contain business-specific factory methods — those belong in their respective interaction classes.
- **ShaderProgramManager** - singleton registry of compiled shader programs (three types: `SPT_BACKGROUND`, `SPT_MESH`, `SPT_GIZMO`)
- **ArcBallRotator** - arcball rotation math for mouse interaction
- **ColorButton** - custom `QLabel`-based color picker widget; emits `colorChanged` signal on user selection
- Shaders are compiled at runtime from inline source strings (no external shader files)

The application entry point (`main.cpp`) sets up an OpenGL 3.3 Core profile surface format with a 24-bit depth buffer before creating the main window. **GLAD initialization** happens in `MeshWidget::initializeGL()` via `GLApiInit()` (`kernel/gl/gl_api_init.h`), which calls `gladLoadGLLoader` with Qt's context function pointer. All OpenGL calls must happen after GLAD is initialized — typically in or after `initializeGL()`.

## Key Patterns

### Vertex type hierarchy

CPU vertex types follow a suffix convention indicating which attributes are present:
- `CpuVertexP` — position only
- `CpuVertexPN` — position + normal
- `CpuVertexPC` — position + color
- `CpuVertexPNC` — position + normal + color (`CpuVertex` is an alias for this)

The corresponding GPU types are `GpuVertexP`, `GpuVertexPN`, `GpuVertexPC`, `GpuVertexPNC`. CPU types are defined in `kernel/core/cpu_vertex.h`; GPU types in `kernel/gl/gpu_vertex.h`.

`VertexTraits<T>` (in `cpu_vertex.h`) provides compile-time attribute metadata (`has_normal`, `has_color`, attribute location indices). Template containers (`CpuTriangleMesh<V>`, `GpuTriangleMesh<V>`, etc.) are parameterized on vertex type.

### Data flow: PLY → CPU → GPU → OpenGL

```
PLY file → LoadPlyAsCpuPolygonMesh → CpuTriangleMesh
                                          │
                                     cpu_to_gpu.cpp
                                          │
                                          ▼
RenderableObject ←── GpuTriangleMesh ─→ GLTriangleMesh (implements GLDrawable)
       │                                        │
       ▼                                        ▼
  Material.bind() ────────→ ShaderProgram ──→ glDrawElements()
```

### Rendering pipeline

`RenderableObject` (in `mesh_studio/renderable_object.h`) is the bridge struct connecting a `GLDrawable` to its `Material`, model matrix, bounds, and shader. `MeshWidget::paintGL()` iterates all `RenderableObject` instances from the `SceneManager` (both permanent and temporary objects): for each one, it calls `Material::bind()` (which sets shader uniforms for color, point size, render mode, lighting) then calls `drawable->draw()`.

### Scene object management

`SceneManager` (in `mesh_studio/scene_manager.h`) manages all renderable objects, separating this concern from `MeshWidget`. Key concepts:

- **Permanent objects** — completed meshes, polylines, annotations. Stored in `objects_` map keyed by `QString` uid. Added via `addObject()`, removed via `removeObject()`.
- **Temporary objects** — drawing previews, transient annotations. Stored in `temp_objects_` map keyed by category string. Only one per category; `setTempObject()` replaces the previous. `finalizeTempObject()` promotes a temp object to permanent.
- **Factory methods** — business-specific object creation (e.g. `DrawPolylineInteraction::createPolyline()`) lives in the respective interaction class, not in SceneManager. SceneManager only provides generic `setTempObject()`/`clearTempObject()`/`finalizeTempObject()` for lifecycle management.
- **GL context** — `createPolyline()` calls OpenGL (via `upload()`), so it must be wrapped in `ctx.runInGLContext(...)`. `ctx.request_update_func()` triggers a repaint after modifying scene objects.
- **Thread safety** — all public methods lock a `std::mutex`.

**Screen-to-world picking** is a two-step process split across layers:
1. `MeshWidget::readDepthPixel(screen_x, screen_y)` — reads the depth buffer via `GLPicker::ReadDepth()` (requires GL context, manages `makeCurrent()`/`doneCurrent()`)
2. `Renderer::pickWorldPoint(screen_x, screen_y, depth, model_matrix)` → `std::optional<glm::vec3>` — validates depth and calls `unprojectWithDepth()` for the coordinate transformation (pure math, no GL calls)

`MouseInteractionContext` exposes both via `readDepthFunc` (a `std::function<float(int,int)>`) and a `renderer` pointer, so interaction code (e.g. `DrawPolylineInteraction`) can compose them without directly depending on `MeshWidget` for picking logic.

> **Removed:** `MeshWidget::updateTempPolyline()`, `clearTempPolyline()`, and `finalizeTempPolyline()` have been removed. Use `SceneManager::setTempObject()` / `clearTempObject()` / `finalizeTempObject()` directly from interaction code. `SceneManager::createPolyline()` has been moved to `DrawPolylineInteraction::createPolyline()`. `MeshWidget::pickWorldPoint()` has been replaced by `Renderer::pickWorldPoint()` + `MeshWidget::readDepthPixel()`.

### Mouse interaction (event handling abstraction)

Mouse events in `MeshWidget` are delegated to an `IMouseInteraction` implementation (strategy pattern). `MouseInteractionContext` bundles all state the handler needs: camera, arcball rotator, gizmo, widget dimensions, scene manager, renderer, `readDepthFunc` (depth buffer reading), `runInGLContext` (executes code block with active GL context), and a `requestUpdate` callback. This decouples interaction logic from the widget, enabling different interaction modes.

Two implementations exist:
- **OrbitInteraction** (`orbit_interaction.h`/`.cpp`) — default mode: orbit/pan/zoom + gizmo axis picking for axis-constrained view manipulation.
- **DrawPolylineInteraction** (`draw_polyline_interaction.h`/`.cpp`) — polyline drawing mode: left-click picks world-space points on mesh surfaces (via depth-buffer unprojection), right-click or Enter finalizes the polyline, Esc cancels, Ctrl+Z undoes last point. The preview is rendered via `SceneManager` temp objects.

New interaction modes can be added by implementing `IMouseInteraction` without modifying `MeshWidget`. Call `MeshWidget::setMouseInteraction()` to switch modes. Note: `onKeyPress` has a default empty implementation in the interface — only override it if your interaction mode needs keyboard input.

Key files: `mouse_interaction.h` (interface + context), `orbit_interaction.h`/`.cpp`, `draw_polyline_interaction.h`/`.cpp`.

### Shader management

`ShaderProgramManager` is a singleton that owns compiled `ShaderProgram` instances keyed by `ProgramTypeId` (an alias for `uint32`). Three shader categories exist (`common.h`): `SPT_BACKGROUND` (gradient sky), `SPT_MESH` (model rendering with lighting), `SPT_GIZMO` (trackball axis rings). Shader GLSL source strings are defined in `mesh_widget.cpp`.

### Material and rendering state

`Material` (`mesh_studio/material.h`) bundles per-object rendering state bound to the shader before each draw call via `Material::bind()`:

| Field | Type | Purpose |
|-------|------|---------|
| `shader_prog_id` | `ProgramTypeId` | Which shader program to use (`SPT_BACKGROUND`/`SPT_MESH`/`SPT_GIZMO`) |
| `base_color` | `glm::vec3` | Uniform color when `use_vertex_color` is false |
| `point_size` | `float` | GL point size for point-cloud rendering |
| `line_width` | `float` | GL line width for wireframe/polyline rendering |
| `use_vertex_color` | `bool` | If true, uses per-vertex color from the mesh data (default) |
| `render_mode` | `MeshRenderMode` | `Facet`, `Wireframe`, or `Points` |

`common.h` also defines `ShaderProgramType` enum, `MeshRenderMode` enum class, and utilities: `ToColor(glm::vec3)` → `QColor`, `GlmVec3ToString(glm::vec3)` → `std::string`.

### TrackballGizmo

`GLTrackballGizmo` (`kernel/gl/gl_trackball_gizmo.h`) renders three colored axis rings (X=red/Y=green/Z=blue) using separate `VertexArray`/`VertexBuffer` pairs. `OrbitInteraction` handles mouse hover detection via `pickGizmoAxis()` to highlight the ring under the cursor, enabling axis-constrained view manipulation.

### Polyline

`GLPolyline` (`kernel/gl/gl_polyline.h`) renders line strip geometry. CPU side uses `CpuPolyline<V>`, GPU side uses `GpuPolyline<V>`. Unlike triangle meshes, polylines use `GL_LINE_STRIP` (or `GL_LINES`) and support configurable line width via `Material::line_width`. `drawWithPoints()` renders both line segments and control points (as GL_POINTS).

`DrawPolylineInteraction::createPolyline()` creates a polyline end-to-end: it builds `GpuPolyline<GpuVertexPC>` (position + color vertices), generates index pairs for each segment, uploads to a `GLPolyline` via `upload()`, and returns a `RenderableObject` with `SPT_MESH` shader, `Wireframe` render mode, and `use_vertex_color = true`. The GPU usage hint is `GL_STATIC_DRAW` (0x88E4). **Must be called with a current GL context** — use `MouseInteractionContext::runInGLContext` to wrap the call.

### Resource management

Qt resources (QSS stylesheets in `res/qss/`, images in `res/image/`) are compiled via `res/resource.qrc`. `resource_util.h` provides `ApplyWidgetStyleSheet()` and `ReadStyleSheetFiles()` for loading QSS at runtime.

### Renderer

`Renderer` (`render/renderer.h`) provides VTK/OSG-inspired rendering management:

**Viewport management** — supports both normalized [0,1] coordinates (like VTK's `vtkViewport`) and pixel coordinates. Call `setViewport()` for normalized or `setViewportInPixels()` for pixel-based. Use `getViewportInPixels()` to retrieve pixel coordinates for `glViewport()`.

**Coordinate system conversions** — three coordinate systems (World, View, Display) with conversion methods:
- `worldToDisplay()` / `displayToWorld()` — 3D world ↔ 2D screen pixels (VTK-style)
- `worldToView()` / `viewToWorld()` — world ↔ camera view space
- `convertCoordinate()` — generic converter between any two systems

**Ray casting** — `computePickRay(screen_x, screen_y)` generates a picking ray from screen coordinates (OSG-style). `unprojectWithDepth()` performs precise unprojection given a screen point, depth value, and optional model matrix — used for mesh-surface picking.

**Multi-layer rendering** — `setLayer(int)` for rendering order (higher = rendered later), `setTransparent(bool)` for overlay layers.

**Background** — `setBackground()` for solid color, `setBackgroundGradient()` for top-to-bottom gradient.

**Important:** Renderer does NOT call OpenGL functions directly (to avoid linking issues with GLAD in the DLL). The caller must apply viewport/background using the getter methods. See `renderer_usage.md` for complete examples.

## Logging

Use the `GLMESH_LOG_*` macros in kernel code:

```cpp
GLMESH_LOG_TRACE("detail: {}", value);
GLMESH_LOG_DEBUG(...);
GLMESH_LOG_INFO(...);
GLMESH_LOG_WARN(...);
GLMESH_LOG_ERROR(...);
GLMESH_LOG_CRITICAL(...);

GLMESH_ASSERT(condition, "message with {}", fmt_args);
```

In mesh_studio code, use the `APP_LOG_*` macros from `mesh_studio/app_log.h` instead. Call `details::InitLogger()` once at startup (done in `main.cpp`). Also available:
- `APP_ASSERT(cond, ...)` — conditional critical log
- `QStrToLogStr(qstring)` — convert QString for logging
- `GlmVec3ToStr(vec)` — format glm::vec3 for logging

## Conventions

- Namespace: `glmesh`, accessed via `GLMESH_NAMESPACE_BEGIN` / `GLMESH_NAMESPACE_END` macros (defined in `glmesh_nsp.h`)
- Foundation type aliases in `glmesh_basedef.h`: `glmesh::int8`, `glmesh::uint32`, `glmesh::Int32Opt`, etc.
- Public type forward declarations in `glmesh_kernel_typedef.h` (included by kernel headers that need type visibility)
- DLL export macros: `GLMESH_KERNEL_API`, `GLMESH_RENDER_API` (defined when `GLMESH_KERNEL_EXPORT` / `GLMESH_RENDER_EXPORT` is set during compilation)
- Include paths use the form `#include "glmesh/kernel/..."` or `#include "glmesh/render/..."`
- Header guards use `__filename_h__` pattern
- Qt resources (QSS stylesheets, images) are in `res/` and compiled via `resource.qrc`
- Display modes: `MeshRenderMode::Facet`, `MeshRenderMode::Wireframe`, `MeshRenderMode::Points`
- There is no test framework in this project
- UIDs for scene objects are `QString` (generated via `QUuid` if not provided)
