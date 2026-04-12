/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: gl_point_cloud.cpp
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
#include "gl_point_cloud.h"

GLMESH_NAMESPACE_BEGIN

void GLPointCloud::upload(const std::vector<GpuVertex>& points, GLenum usage)
{
    vao_.bind();
    vbo_.upload(points.data(), points.size() * sizeof(GpuVertex), usage);

    GpuVertex::SetupAttribs();
    point_count_ = points.size();
}

void GLPointCloud::draw() const noexcept
{
    vao_.bind();
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(point_count_));
}

GLMESH_NAMESPACE_END