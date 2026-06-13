/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gl_picker.h
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
#ifndef __gl_picker_h__
#define __gl_picker_h__

#include "glmesh/kernel/glmesh_kernel_export.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_KERNEL_API GLPicker
{
public:
    /**
     * @brief 读取指定屏幕位置的深度值
     * @param x 屏幕 X 坐标（左上角为原点，Qt 风格）
     * @param y 屏幕 Y 坐标（左上角为原点，Qt 风格）
     * @param viewport_height 视口高度，用于 Y 轴翻转（OpenGL Y 轴从下到上）
     * @return 深度值 [0.0 (近裁剪面), 1.0 (远裁剪面)]
     */
    static float ReadDepth(int x, int y, int viewport_height);

    /**
     * @brief 检查深度值是否有效（是否拾取到几何体）
     * @param depth 深度值
     * @return true 表示拾取到几何体，false 表示背景（远裁剪面）
     */
    static bool IsValidDepth(float depth);
};

GLMESH_NAMESPACE_END

#endif
