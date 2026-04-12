/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: gl_triangle_mesh.cpp
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
#include "gl_triangle_mesh.h"
#include "glad.h"

GLMESH_NAMESPACE_BEGIN

void GLTriangleMesh::upload(const std::vector<GpuVertex>& vertices, const std::vector<uint32_t>& indices, uint32_t usage)
{
    index_count_ = indices.size();
    vao_.bind();
    vbo_.upload(vertices.data(), vertices.size() * sizeof(GpuVertex), usage);
    ebo_.upload(indices.data(), indices.size() * sizeof(uint32_t), usage);
    GpuVertex::SetupAttribs();
    vao_.unbind();    
}

void GLTriangleMesh::draw() const noexcept
{
    vao_.bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(index_count_),
        GL_UNSIGNED_INT,
        nullptr
    );
}

GLMESH_NAMESPACE_END