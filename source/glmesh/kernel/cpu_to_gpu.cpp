/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: cpu_to_gpu.cpp
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
#include "cpu_to_gpu.h"

GLMESH_NAMESPACE_BEGIN

GpuBkgVertex ToGpuBkgVertex(const CpuBkgVertex& v)
{
    return {
        .position = v.position,
        .color    = v.color
    };
}

GpuBkg ToGpuBkg(const CpuBkg& bkg)
{
    GpuBkg gpu_bkg;
    gpu_bkg.vertices[0] = ToGpuBkgVertex(bkg.left_bottom_vertex);
    gpu_bkg.vertices[1] = ToGpuBkgVertex(bkg.right_bottom_vertex);
    gpu_bkg.vertices[2] = ToGpuBkgVertex(bkg.right_top_vertex);
    gpu_bkg.vertices[3] = ToGpuBkgVertex(bkg.left_top_vertex);
    return gpu_bkg;
}

GLMESH_NAMESPACE_END