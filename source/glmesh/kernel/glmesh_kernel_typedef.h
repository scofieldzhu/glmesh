/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: glmesh_kernel_typedef.h
 *  Copyright (c) 2024-2024 scofieldzhu
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
#ifndef __glmesh_kernel_typedef_h__
#define __glmesh_kernel_typedef_h__

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "glmesh/glmesh_basedef.h"
#include "glmesh/kernel/glmesh_kernel_export.h"

GLMESH_NAMESPACE_BEGIN

// CPU vertex types
struct CpuVertexP;
struct CpuVertexPN;
struct CpuVertexPC;
struct CpuVertexPNC;

// GPU vertex types
struct GpuVertexP;
struct GpuVertexPN;
struct GpuVertexPC;
struct GpuVertexPNC;

// CPU container types
template<typename V> struct CpuTriangleMesh;
template<typename V> struct CpuPointCloud;
template<typename V> struct CpuPolyline;
template<typename V> struct CpuRectangle;
struct CpuPolygonMesh;

// GPU container types
template<typename V> struct GpuTriangleMesh;
template<typename V> struct GpuPolyline;
template<typename V> struct GpuRectangle;
struct GpuBkgVertex;
struct GpuBkg;

// GL drawable classes
struct GLDrawable;
class GLTriangleMesh;
class GLPointCloud;
class GLRectangle;
class GLPolyline;
class GLTrackballGizmo;
class GLBkg;

// Support classes
class IndexBuffer;
class ShaderProgram;
class VertexArray;
class VertexBuffer;

// Geometry
struct Bounds3D;

GLMESH_NAMESPACE_END

#endif