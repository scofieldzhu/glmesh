/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: arcball_rotator.cpp
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
#include "arcball_rotator.h"

void ArcBallRotator::onUpdateMousePos(QMouseEvent* event, const QSize& event_widget_size)
{
    // 获取当前鼠标在球体上的三维向量
    glm::vec3 current_arcball_vec = getArcballVector(event->pos(), event_widget_size.width(), event_widget_size.height());
    
    // 计算两个向量的夹角余弦值
    float dot_prod = glm::dot(last_arcball_vec_, current_arcball_vec);
    // 防止浮点数精度问题导致 acos 越界返回 NaN
    dot_prod = glm::clamp(dot_prod, -1.0f, 1.0f); 
    
    // 计算旋转角度
    float angle = std::acos(dot_prod);
    
    if (angle > 1e-5f) { // 如果确实发生了移动
        // 通过叉乘得到旋转轴 (右手定则)
        glm::vec3 axis = glm::cross(last_arcball_vec_, current_arcball_vec);
        axis = glm::normalize(axis);
        
        // 构建这一次移动产生的旋转四元数
        glm::quat delta_rotation = glm::angleAxis(angle, axis);
        
        // 将本次旋转叠加到总旋转上 (注意乘法顺序)
        model_rotation_ = delta_rotation * model_rotation_;
    }
    
    // 更新记录，用于下一帧计算
    last_arcball_vec_ = current_arcball_vec;
        
}

void ArcBallRotator::onStartRotationEvent(QMouseEvent *event, const QSize &event_widget_size)
{
    last_arcball_vec_ = getArcballVector(event->pos(), event_widget_size.width(), event_widget_size.height());
}

glm::mat4 ArcBallRotator::getRotationMat() const
{
    return glm::mat4_cast(model_rotation_);
}

glm::vec3 ArcBallRotator::getArcballVector(const QPoint& pt, const int widget_width, const int widget_height)const
{
    // 1. 将鼠标坐标归一化到 [-1, 1] 的 NDC 空间
    // 注意：Qt的Y轴向下，OpenGL的Y轴向上，所以Y要做 1.0 - ... 的反转
    float x = (2.0f * pt.x() / widget_width) - 1.0f;
    float y = 1.0f - (2.0f * pt.y() / widget_height); 
    
    glm::vec3 P(x, y, 0.0f);
    
    // 2. 计算点到屏幕中心的距离的平方
    float OP_squared = x * x + y * y;
    
    if (OP_squared <= 1.0f) {
        // 如果点在球体内（勾股定理求 Z）
        P.z = std::sqrt(1.0f - OP_squared); 
    } else {
        // 如果点在球体外，把它拉回球面上（Z = 0）
        P = glm::normalize(P); 
    }
    
    return P;
}