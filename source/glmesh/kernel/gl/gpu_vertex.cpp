/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gpu_vertex.cpp
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
#include "gpu_vertex.h"
#include <glad/glad.h>

GLMESH_NAMESPACE_BEGIN

void GpuVertexP::SetupAttribs()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexP),
        reinterpret_cast<void*>(offsetof(GpuVertexP, position))
    );
}

void GpuVertexPN::SetupAttribs()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPN),
        reinterpret_cast<void*>(offsetof(GpuVertexPN, position))
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPN),
        reinterpret_cast<void*>(offsetof(GpuVertexPN, normal))
    );
}

void GpuVertexPC::SetupAttribs()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPC),
        reinterpret_cast<void*>(offsetof(GpuVertexPC, position))
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPC),
        reinterpret_cast<void*>(offsetof(GpuVertexPC, color))
    );
}

void GpuVertexPNC::SetupAttribs()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPNC),
        reinterpret_cast<void*>(offsetof(GpuVertexPNC, position))
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPNC),
        reinterpret_cast<void*>(offsetof(GpuVertexPNC, normal))
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 
        3, 
        GL_FLOAT, 
        GL_FALSE,
        sizeof(GpuVertexPNC),
        reinterpret_cast<void*>(offsetof(GpuVertexPNC, color))
    );
}

GLMESH_NAMESPACE_END