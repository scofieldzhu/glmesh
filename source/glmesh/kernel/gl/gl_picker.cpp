/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gl_picker.cpp
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
#include "gl_picker.h"
#include "glad/glad.h"

GLMESH_NAMESPACE_BEGIN

float GLPicker::ReadDepth(int x, int y, int viewport_height)
{
    // OpenGL 的 Y 轴从下到上，Qt 的 Y 轴从上到下
    // 需要翻转 Y 坐标
    int gl_y = viewport_height - y - 1;

    float depth = 1.0f;
    glReadPixels(x, gl_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    return depth;
}

bool GLPicker::IsValidDepth(float depth)
{
    // 深度值接近 1.0 表示远裁剪面，即没有拾取到任何几何体
    // 使用小的容差值来判断
    return depth < 0.9999f;
}

GLMESH_NAMESPACE_END
