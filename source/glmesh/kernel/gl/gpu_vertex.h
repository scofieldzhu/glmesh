/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gpu_vertex.h
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
#ifndef __gpu_vertex_h__
#define __gpu_vertex_h__

#include "glmesh/kernel/glmesh_kernel_typedef.h"
#include "glmesh/kernel/core/cpu_vertex.h"

GLMESH_NAMESPACE_BEGIN

struct GLMESH_KERNEL_API GpuVertexP
{
    static void SetupAttribs();
    glm::vec3 position;
};

struct GLMESH_KERNEL_API GpuVertexPN
{
    static void SetupAttribs();
    glm::vec3 position;
    glm::vec3 normal{0.0f, 0.0f, 1.0f};
};

struct GLMESH_KERNEL_API GpuVertexPC
{
    static void SetupAttribs();
    glm::vec3 position;
    glm::vec3 color{1.0f, 1.0f, 1.0f};
};

struct GLMESH_KERNEL_API GpuVertexPNC
{
    static void SetupAttribs();
    glm::vec3 position;
    glm::vec3 normal{0.0f, 0.0f, 1.0f};
    glm::vec3 color{1.0f, 1.0f, 1.0f};
};

using GpuVertex = GpuVertexPNC;

template<>
struct VertexTraits<GpuVertexP>
{
    static constexpr bool has_normal = false;
    static constexpr bool has_color  = false;
    static constexpr uint32 pos_location    = 0;
    static constexpr uint32 normal_location = 1;
    static constexpr uint32 color_location  = 2;
};

template<>
struct VertexTraits<GpuVertexPN>
{
    static constexpr bool has_normal = true;
    static constexpr bool has_color  = false;
    static constexpr uint32 pos_location    = 0;
    static constexpr uint32 normal_location = 1;
    static constexpr uint32 color_location  = 2;
};

template<>
struct VertexTraits<GpuVertexPC>
{
    static constexpr bool has_normal = false;
    static constexpr bool has_color  = true;
    static constexpr uint32 pos_location    = 0;
    static constexpr uint32 normal_location = 1;
    static constexpr uint32 color_location  = 2;
};

template<>
struct VertexTraits<GpuVertexPNC>
{
    static constexpr bool has_normal = true;
    static constexpr bool has_color  = true;
    static constexpr uint32 pos_location    = 0;
    static constexpr uint32 normal_location = 1;
    static constexpr uint32 color_location  = 2;
};

GLMESH_NAMESPACE_END

#endif