/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gl_polyline.h
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
#ifndef __gl_polyline_h__
#define __gl_polyline_h__

#include "glmesh/kernel/gl/gl_drawable.h"
#include "glmesh/kernel/gl/vertex_array.h"
#include "glmesh/kernel/gl/vertex_buffer.h"
#include "glmesh/kernel/gl/index_buffer.h"

GLMESH_NAMESPACE_BEGIN

template<typename V> struct GpuPolyline;

class GLPolyline : public GLDrawable
{
public:
    void draw() const override;
    template<typename V>
    void upload(const GpuPolyline<V>& gpu_line, uint32 usage);
    bool valid() const { return uploaded_; }

private:
    VertexArray vao_;
    VertexBuffer vbo_;
    IndexBuffer ebo_;
    std::size_t index_count_ = 0;
    bool uploaded_ = false;
};

template<typename V>
void GLPolyline::upload(const GpuPolyline<V>& gpu_line, uint32 usage)
{
    index_count_ = gpu_line.indexes.size();
    vao_.bind();
    vbo_.upload(gpu_line.vertexes.data(), gpu_line.vertexes.size() * sizeof(V), usage);
    ebo_.upload(gpu_line.indexes.data(), gpu_line.indexes.size() * sizeof(uint32), usage);
    V::SetupAttribs();
    vao_.unbind();
    uploaded_ = true;
}

GLMESH_NAMESPACE_END

#endif