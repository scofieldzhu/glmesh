# GLMESH.md

## Project Overview

GLMesh is a C++20 mesh data rendering library built on OpenGL 3.3+ and Qt5. It wraps the OpenGL API with object-oriented abstractions for rendering 3D mesh and point cloud data loaded from PLY files.

## Build Commands

Requires: Visual Studio 2022, Qt5 (msvc2017_64), vcpkg, CMake 3.15+. Local paths are configured in `CMakeUserPresets.json`.

```bash
# Configure (uses local vcpkg toolchain + Qt paths)
cmake --preset vs2022-local

# Build Debug
cmake --build --preset build-debug-local

# Build Release
cmake --build --preset build-release-local
```

Output binaries land in `build/vs2022-local/bin/`.

## Testing

There is no automated test framework. Testing is manual via a GUI executable:

- `MeshStudio.exe` — main interactive application

## Architecture

### Key Abstraction Layers

**CPU layer** (`kernel/core/`): In-memory mesh representations loaded from disk.
- `CpuPolygonMesh`, `CpuTriangleMesh`, `CpuPointCloud` — data containers
- `CpuVertex` — position, normal, color per vertex
- `MeshBounds` — bounding box computation
- `GeometryUtils` — geometric calculations

**IO layer** (`kernel/io/`): File loading.
- `MeshLoader` — loads PLY files, returns `CpuPolygonMesh`

**GPU conversion** (`kernel/cpu_to_gpu.h/cpp`): Translates CPU structures to GPU-ready formats.
- `ToGpuVertex()`, `ToGpuVertices()`, `ToGpuTriangleMesh()`

**GPU/GL layer** (`kernel/gl/`): OpenGL resource management and rendering.
- `GLTriangleMesh`, `GLPointCloud` — GPU-side mesh data bound to VAO/VBO/EBO
- `ShaderProgram` — GLSL compilation and uniform management
- `VertexArray`, `VertexBuffer`, `IndexBuffer` — RAII wrappers for OpenGL objects
- `GLDrawable` — abstract rendering interface
- `GLApiInit` — initializes glad (OpenGL loader)

**App layer** (`app/`): Qt5 interactive viewer.
- `MeshWidget` — `QOpenGLWidget` subclass; handles OpenGL init, paint loop, and arcball camera (quaternion-based rotation via mouse + scroll)
- `RenderableObject` — wrapper pairing a `GLDrawable` with transform/material state
- `MainWidget` — top-level Qt window

### Data Flow

```
PLY file → MeshLoader → CpuPolygonMesh → CpuTriangleMesh
         → cpu_to_gpu → GLTriangleMesh + ShaderProgram
         → MeshWidget (RenderableObject collection) → OpenGL display
```

### Module Structure

The build produces a shared library (`glmesh.kernel`) consumed by the app and test executables. The legacy `glmesh.core` and `glmesh.test` modules are disabled (commented out in root `CMakeLists.txt`).

### Export Macro

Public kernel API symbols use `GLMESH_KERNEL_API` (defined in `glmesh_basedef.h`), which expands to `__declspec(dllexport/dllimport)` on Windows.

### Logging

`GlmeshLog` wraps spdlog. Use the provided macros (`GLMESH_LOG_*`) rather than calling spdlog directly.

### Namespace

All library code lives under a namespace controlled by macros in `glmesh_nsp.h`.
