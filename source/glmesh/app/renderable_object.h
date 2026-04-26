/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: renderable_object.h
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
#ifndef __renderable_object_h__
#define __renderable_object_h__

#include "glmesh/kernel/gl/gl_drawable.h"

struct RenderableObject 
{
    // 泛化：现在它可以容纳 Mesh、线段、点云、基本几何体
    std::shared_ptr<glmesh::GLDrawable> drawable;
    
    // 空间变换
    glm::mat4 model_matrix{1.0f};
    
    // 基础材质/渲染属性
    glm::vec4 base_color{0.8f, 0.8f, 0.8f, 1.0f}; // 换成 vec4 支持透明度更好
    float line_width{1.0f};
    float point_size{1.0f};
    
    // 状态控制
    bool visible{true};
    // 甚至可以加一个枚举来标识要用哪个 Shader
    // enum ShaderType { StandardLighting, UnlitColor, VertexColor } shader_type;
};

#endif