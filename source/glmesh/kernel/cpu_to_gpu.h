/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: cpu_to_gpu.h
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
#ifndef __cpu_to_gpu_h__
#define __cpu_to_gpu_h__

#include "glmesh/kernel/core/cpu_triangle_mesh.h"
#include "glmesh/kernel/core/cpu_rectangle.h"
#include "glmesh/kernel/core/cpu_point_cloud.h"
#include "glmesh/kernel/core/cpu_polyline.h"
#include "glmesh/kernel/gl/gpu_triangle_mesh.h"
#include "glmesh/kernel/gl/gpu_rectangle.h"
#include "glmesh/kernel/gl/gpu_polyline.h"
#include "glmesh/kernel/gl/gpu_bkg.h"
#include "glmesh/kernel/core/cpu_bkg.h"

GLMESH_NAMESPACE_BEGIN

template<typename GpuV, typename CpuV>
GpuV ToGpuVertex(const CpuV& v)
{
    GpuV out;
    out.position = v.position;
    if constexpr (VertexTraits<CpuV>::has_normal && VertexTraits<GpuV>::has_normal){
        out.normal = v.normal;
    }
    if constexpr (VertexTraits<CpuV>::has_color){
        if constexpr (VertexTraits<GpuV>::has_color){
            out.color = v.color;
        }
    }
    return out;
}

template<typename GpuV, typename CpuV>
std::vector<GpuV> ToGpuVertices(const std::vector<CpuV>& src)
{
    std::vector<GpuV> dst;
    dst.reserve(src.size());
    for(const auto& v : src){
        dst.push_back(ToGpuVertex<GpuV>(v));
    }
    return dst;
}

template<typename GpuV, typename CpuV>
GpuTriangleMesh<GpuV> ToGpuTriangleMesh(const CpuTriangleMesh<CpuV>& tri_mesh)
{
    return {
        .vertices = ToGpuVertices<GpuV>(tri_mesh.vertices),
        .indices  = tri_mesh.indices
    };
}

template<typename GpuV, typename CpuV>
GpuRectangle<GpuV> ToGpuRectangle(const CpuRectangle<CpuV>& rt)
{
    return {
        {
            ToGpuVertex<GpuV>(rt.left_bottom_point),
            ToGpuVertex<GpuV>(rt.right_bottom_point),
            ToGpuVertex<GpuV>(rt.right_top_point),
            ToGpuVertex<GpuV>(rt.left_top_point)
        }
    };
}

template<typename GpuV, typename CpuV>
GpuPolyline<GpuV> ToGpuPolyline(const CpuPolyline<CpuV>& pl)
{
    GpuPolyline<GpuV> out;
    out.vertexes = ToGpuVertices<GpuV>(pl.vertexes);
    return out;
}

// Convenience overloads for the common case: CpuVertex (PNC) -> GpuVertex (PNC)
inline GpuVertex ToGpuVertex(const CpuVertex& v)
{
    return ToGpuVertex<GpuVertex>(v);
}

inline std::vector<GpuVertex> ToGpuVertices(const std::vector<CpuVertex>& src)
{
    return ToGpuVertices<GpuVertex>(src);
}

inline GpuTriangleMesh<GpuVertex> ToGpuTriangleMesh(const CpuTriangleMesh<CpuVertex>& tri_mesh)
{
    return ToGpuTriangleMesh<GpuVertex>(tri_mesh);
}

inline GpuRectangle<GpuVertex> ToGpuRectangle(const CpuRectangle<CpuVertex>& rt)
{
    return ToGpuRectangle<GpuVertex>(rt);
}

inline GpuPolyline<GpuVertex> ToGpuPolyline(const CpuPolyline<CpuVertex>& pl)
{
    return ToGpuPolyline<GpuVertex>(pl);
}

// Non-template: background vertex types have fixed layout (position + color)
GLMESH_KERNEL_API GpuBkgVertex ToGpuBkgVertex(const CpuBkgVertex& v);
GLMESH_KERNEL_API GpuBkg ToGpuBkg(const CpuBkg& bkg);

GLMESH_NAMESPACE_END

#endif