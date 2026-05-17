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
```

Output binaries go to `build/vs2022-local/bin/`.

## Dependencies

- **Qt5** (Core, Widgets, OpenGL) - UI framework and OpenGL widget hosting
- **PCL** (Point Cloud Library) - mesh/point cloud I/O (PLY loading)
- **GLM** - math (vectors, matrices, quaternions); used with `GLM_ENABLE_EXPERIMENTAL`
- **spdlog** - logging (trace level active)
- **GLAD** - OpenGL 3.0+ function loader (bundled in `source/glmesh/kernel/third_party/glad/`)

All external deps except GLAD are resolved via vcpkg. Qt5 is found via `CMAKE_PREFIX_PATH`.

## Architecture

The project is a C++20 OpenGL mesh viewer with three modules under `source/glmesh/`:

### kernel (`glmesh.kernel` shared library)
Low-level OpenGL abstractions and data types. Three sub-layers:
- **core/** - CPU-side data structures: `CpuVertex`, `CpuTriangleMesh`, `CpuPointCloud`, `CpuBkg`, `Bounds3D`, geometry utilities
- **gl/** - GPU-side OpenGL wrappers: `VertexArray`, `VertexBuffer`, `IndexBuffer`, `ShaderProgram`, and drawable types (`GLTriangleMesh`, `GLPointCloud`, `GLRectangle`, `GLBkg`) implementing the `GLDrawable` interface
- **io/** - mesh file loading (`LoadPlyAsCpuPolygonMesh`)
- **cpu_to_gpu** - conversion functions from CPU structs to GPU structs

### render (`glmesh.render` shared library)
Higher-level rendering utilities. Currently contains `Camera` (orbit/pan/zoom, perspective/orthographic projection, quaternion-based rotation).

### mesh_studio (`MeshStudio` executable)
Qt5 application that ties everything together:
- `MainWidget` - QMainWindow with UI for importing meshes, tree view, display mode controls, lighting parameters
- `MeshWidget` - QOpenGLWidget that manages the GL context, camera interaction (arcball rotation, pan, zoom), background gradient, and renders `RenderableObject` instances
- `ShaderProgramManager` - singleton registry of compiled shader programs
- `ArcBallRotator` - arcball rotation math for mouse interaction
- Shaders are compiled at runtime from inline source strings (no external shader files)

## Conventions

- Namespace: `glmesh`, accessed via `GLMESH_NAMESPACE_BEGIN` / `GLMESH_NAMESPACE_END` macros
- DLL export macros: `GLMESH_KERNEL_API`, `GLMESH_RENDER_API` (defined when `GLMESH_KERNEL_EXPORT` / `GLMESH_RENDER_EXPORT` is set during compilation)
- Include paths use the form `#include "glmesh/kernel/..."` or `#include "glmesh/render/..."`
- Header guards use `__filename_h__` pattern
- Qt resources (QSS stylesheets, images) are in `res/` and compiled via `resource.qrc`
