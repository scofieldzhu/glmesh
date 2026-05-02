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
#include "glmesh/kernel/core/cpu_rectangle.h"
#include "glmesh/kernel/core/cpu_bkg.h"

GLMESH_NAMESPACE_BEGIN

GpuVertex ToGpuVertex(const CpuVertex& v)
{
    return {
        .position = v.position,
        .normal   = v.normal,
        .color    = v.color
    };
}

std::vector<GpuVertex> ToGpuVertices(const std::vector<CpuVertex>& src)
{
    std::vector<GpuVertex> dst;
    dst.reserve(src.size());
    for(const auto& v : src){
        dst.push_back(ToGpuVertex(v));
    }
    return dst;  
}

GpuTriangleMesh ToGpuTriangleMesh(const CpuTriangleMesh& triangle_mesh)
{
    GpuTriangleMesh gpu_triangle_mesh;
    gpu_triangle_mesh.vertices = ToGpuVertices(triangle_mesh.vertices);
    gpu_triangle_mesh.indices = triangle_mesh.indices;
    return gpu_triangle_mesh;
}

GpuRectangle ToGpuRectangle(const CpuRectangle& rt)
{
    GpuRectangle gpu_rt;
    gpu_rt.vertices[0] = ToGpuVertex(rt.left_bottom_point);
    gpu_rt.vertices[1] = ToGpuVertex(rt.right_bottom_point);
    gpu_rt.vertices[2] = ToGpuVertex(rt.right_top_point);
    gpu_rt.vertices[3] = ToGpuVertex(rt.left_top_point);
    return gpu_rt;
}

GpuBkg ToGpuBkg(const CpuBkg &bkg)
{
    GpuBkg gpu_bkg;
    gpu_bkg.vertices[0] = ToGpuBkgVertex(bkg.left_bottom_vertex);
    gpu_bkg.vertices[1] = ToGpuBkgVertex(bkg.right_bottom_vertex);
    gpu_bkg.vertices[2] = ToGpuBkgVertex(bkg.right_top_vertex);
    gpu_bkg.vertices[3] = ToGpuBkgVertex(bkg.left_top_vertex);    
    return gpu_bkg;
}

GpuBkgVertex ToGpuBkgVertex(const CpuBkgVertex &v)
{
    return {
        .position = v.position,
        .color    = v.color
    };
}

GLMESH_NAMESPACE_END
