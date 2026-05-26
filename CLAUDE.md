# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

This project uses CMake with vcpkg for dependency management. The local preset (`vs2022-local`) requires a `CMakeUserPresets.json` that sets `CMAKE_TOOLCHAIN_FILE` to the vcpkg toolchain and `CMAKE_PREFIX_PATH` to the Qt5 installation.

```bash
# Configure
cmake --preset vs2022-local

# Build Debug
cmake --build --preset build-debug-local

# Build Release
cmake --build --preset build-release-local

# Install (copies binaries, DLLs, and headers to CMAKE_INSTALL_PREFIX, default: ./install/)
cmake --install build/vs2022-local --config Debug
cmake --install build/vs2022-local --config Release
```

Output binaries go to `build/vs2022-local/bin/`. The install step runs `windeployqt` to collect Qt5 runtime DLLs alongside vcpkg dependency DLLs.

## Dependencies

- **Qt5** (Core, Widgets, OpenGL) - UI framework and OpenGL widget hosting
- **PCL** (Point Cloud Library) - mesh/point cloud I/O (PLY loading)
- **GLM** - math (vectors, matrices, quaternions); used with `GLM_ENABLE_EXPERIMENTAL`
- **spdlog** - logging (trace level active via `SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE`)
- **GLAD** - OpenGL 3.0+ function loader (bundled in `source/glmesh/kernel/third_party/glad/`)

All external deps except GLAD are resolved via vcpkg. Qt5 is found via `CMAKE_PREFIX_PATH`.

## Architecture

The project is a C++20 OpenGL mesh viewer with three modules under `source/glmesh/`:

### kernel (`glmesh.kernel` shared library)
Low-level OpenGL abstractions and data types. Three sub-layers:
- **core/** - CPU-side data structures: `CpuVertex`, `CpuTriangleMesh`, `CpuPointCloud`, `CpuBkg`, `Bounds3D`, geometry utilities
- **gl/** - GPU-side OpenGL wrappers: `VertexArray`, `VertexBuffer`, `IndexBuffer`, `ShaderProgram`, and drawable types (`GLTriangleMesh`, `GLPointCloud`, `GLRectangle`, `GLBkg`, `GLTrackballGizmo`, `GLPolyline`) implementing the `GLDrawable` interface
- **io/** - mesh file loading (`LoadPlyAsCpuPolygonMesh`)
- **cpu_to_gpu** - conversion functions from CPU structs to GPU structs (e.g. `ToGpuTriangleMesh`, `ToGpuVertex`)

### render (`glmesh.render` shared library)
Higher-level rendering utilities. Currently contains `Camera` (orbit/pan/zoom, perspective/orthographic projection, quaternion-based rotation).

### mesh_studio (`MeshStudio` executable)
Qt5 application that ties everything together:
- **MainWidget** - QMainWindow with UI for importing meshes, tree view, display mode controls, lighting parameters
- **MeshWidget** - QOpenGLWidget that manages the GL context, camera interaction, background gradient, and renders `RenderableObject` instances. Shader source strings are defined here as inline `const char*` literals.
- **ShaderProgramManager** - singleton registry of compiled shader programs (three types: `SPT_BACKGROUND`, `SPT_MESH`, `SPT_GIZMO`)
- **ArcBallRotator** - arcball rotation math for mouse interaction
- Shaders are compiled at runtime from inline source strings (no external shader files)

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

`RenderableObject` (in `mesh_studio/renderable_object.h`) is the bridge struct connecting a `GLDrawable` to its `Material`, model matrix, bounds, and shader. `MeshWidget::paintGL()` iterates all `RenderableObject` instances: for each one, it calls `Material::bind()` (which sets shader uniforms for color, point size, render mode, lighting) then calls `drawable->draw()`.

### Mouse interaction (event handling abstraction)

Mouse events in `MeshWidget` are delegated to an `IMouseInteraction` implementation (strategy pattern). `MouseInteractionContext` bundles all state the handler needs (camera, arcball rotator, gizmo, widget dimensions, an `requestUpdate` callback). This decouples interaction logic from the widget, enabling different interaction modes.

The current implementation is `OrbitInteraction` (orbit/pan/zoom + gizmo axis picking). New interaction modes can be added by implementing `IMouseInteraction` without modifying `MeshWidget`.

Key files: `mouse_interaction.h` (interface + context), `orbit_interaction.h`/`.cpp` (default implementation).

### Shader management

`ShaderProgramManager` is a singleton that owns compiled `ShaderProgram` instances keyed by `ProgramTypeId` (an alias for `uint32`). Three shader categories exist (`common.h`): `SPT_BACKGROUND` (gradient sky), `SPT_MESH` (model rendering with lighting), `SPT_GIZMO` (trackball axis rings). Shader GLSL source strings are defined in `mesh_widget.cpp`.

### TrackballGizmo

`GLTrackballGizmo` (`kernel/gl/gl_trackball_gizmo.h`) renders three colored axis rings (X=red/Y=green/Z=blue) using separate `VertexArray`/`VertexBuffer` pairs. `OrbitInteraction` handles mouse hover detection via `pickGizmoAxis()` to highlight the ring under the cursor, enabling axis-constrained view manipulation.

### Polyline

`GLPolyline` (`kernel/gl/gl_polyline.h`) renders line strip geometry. CPU side uses `CpuPolyline<V>`, GPU side uses `GpuPolyline<V>`. Unlike triangle meshes, polylines use `GL_LINE_STRIP` (or `GL_LINES`) and support configurable line width via `Material::line_width`.

### Resource management

Qt resources (QSS stylesheets in `res/qss/`, images in `res/image/`) are compiled via `res/resource.qrc`. `resource_util.h` provides `ApplyWidgetStyleSheet()` and `ReadStyleSheetFiles()` for loading QSS at runtime.

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

In mesh_studio code, use the `APP_LOG_*` macros from `app_log.h` instead.

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
