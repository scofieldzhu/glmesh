/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: mesh_loader.h
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
#ifndef __mesh_loader_h__
#define __mesh_loader_h__

#include "glmesh/kernel/gl/gl_triangle_mesh.h"
#include "glmesh/kernel/core/cpu_polygon_mesh.h"
#include "glmesh/kernel/core/cpu_triangle_mesh.h"

GLMESH_NAMESPACE_BEGIN

GLMESH_KERNEL_API CpuTriangleMesh Triangulate(const CpuPolygonMesh& polyMesh);

enum class MeshLoadError
{
    None,
    FileNotFound,
    UnsupportedFormat,
    ParseFailed,
    EmptyMesh,
    InvalidTopology
};
GLMESH_KERNEL_API bool LoadPlyAsCpuPolygonMesh(const std::string& ply_filepath, CpuPolygonMesh& out_mesh, MeshLoadError* out_err = nullptr);
GLMESH_KERNEL_API bool LoadPlyRenderableMesh(const std::string& ply_filepath, GLTriangleMesh& out_mesh, MeshLoadError* out_err = nullptr);

GLMESH_NAMESPACE_END

#endif