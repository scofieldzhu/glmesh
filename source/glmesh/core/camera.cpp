/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: camera.cpp 
 *  Copyright (c) 2024-2024 scofieldzhu
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

#include "camera.h"
#include <glm/gtx/transform.hpp>
#include "shader_program.h"

GLMESH_NAMESPACE_BEGIN

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::setModel(const glmMatrix& mat)
{
    model_ = mat;
}

void Camera::setEye(const glmPt3& pt)
{
    eye_ = pt;
}

void Camera::setProjection(const glmMatrix& mat)
{
    projection_ = mat;
}

void Camera::setView(const glmMatrix& mat)
{
    view_ = mat;
}

void Camera::setFocalPoint(const glmPt3 &pt)
{
    focal_point_ = pt;
}

void Camera::setViewUp(const glmNormal &n)
{
    viewup_ = n;
}

void Camera::setWinAspect(float f)
{
    win_aspect_ = f;
}

void Camera::setFarPlaneDist(float dist)
{
    far_plane_dist_ = dist;
}

void Camera::setNearPlaneDist(float dist)
{
    near_plane_dist_ = dist;
}

void Camera::setFovy(float f)
{
    fovy_ = f;
}

void Camera::calcProjection()
{
    projection_ = glm::perspective(glm::radians(fovy_), win_aspect_, near_plane_dist_, far_plane_dist_);  
}

void Camera::calcView()
{
    view_ = glm::lookAt(eye_, focal_point_, viewup_);
}

void Camera::recalc()
{
    calcView();
    calcProjection();
}

void Camera::syncDataToShader(glmShaderProgramPtr prog)
{
    if(prog){
        prog->setUniformMatrix4fv("model", model_);
        prog->setUniformMatrix4fv("view", view_);
        prog->setUniformMatrix4fv("projection", projection_);
    }
}

GLMESH_NAMESPACE_END