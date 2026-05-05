/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: arcball_rotator.h
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
#ifndef __arcball_rotator_h__
#define __arcball_rotator_h__

#include <QMouseEvent>
#include <glm/gtc/quaternion.hpp>
#include "glmesh/kernel/glmesh_kernel_typedef.h"

class ArcBallRotator
{
public:
    void onUpdateMousePos(QMouseEvent* event, const QSize& event_widget_size);
    void onStartRotationEvent(QMouseEvent* event, const QSize& event_widget_size);
    glm::mat4 getRotationMat()const;

private:
    glm::vec3 getArcballVector(const QPoint& pt, const int widget_width, const int widget_height)const;
    glm::vec3 last_arcball_vec_;  
    glm::quat model_rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // 记录累积的旋转（四元数）
};

#endif