/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gl_rectangle.cpp
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
#include "gl_rectangle.h"
#include "glmesh/kernel/gl/gpu_rectangle.h"
#include "glad/glad.h"

GLMESH_NAMESPACE_BEGIN

// Split the quad into two CCW triangles: (0,1,2) and (0,2,3).
namespace{
    constexpr uint32 kRectIndices[6] = {0, 1, 2, 0, 2, 3};
} 

void GLRectangle::upload(const GpuRectangle& rect, uint32 usage)
{
    vao_.bind();
    vbo_.upload(rect.vertices.data(), rect.vertices.size() * sizeof(GpuVertex), usage);
    ebo_.upload(kRectIndices, sizeof(kRectIndices), usage);
    GpuVertex::SetupAttribs();
    vao_.unbind();
    uploaded_ = true;
}

void GLRectangle::draw() const
{
    vao_.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

GLMESH_NAMESPACE_END
