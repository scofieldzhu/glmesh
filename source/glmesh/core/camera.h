/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: camera.h 
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

#ifndef __camera_h__
#define __camera_h__

#include "glmesh/core/glm_base_type.h"
#include "glmesh/core/glm_export.h"
#include "glmesh/core/instantiator.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_API Camera : public Instantiator<Camera>
{
public:
    void setModel(const glmMatrix& mat);
    const auto& model(){ return model_; }
    void setView(const glmMatrix& mat);
    const auto& view(){ return view_; }
    void setProjection(const glmMatrix& mat);
    const auto& projection(){ return projection_; }
    void setEye(const glmPt3& pt);
    const auto& eye()const{ return eye_; }
    void setFocalPoint(const glmPt3& pt);
    const auto& focalPoint()const{ return focal_point_; }
    void setViewUp(const glmNormal& n);
    const auto& viewUp()const{ return viewup_; }
    void setWinAspect(float f);
    auto winAspect()const{ return win_aspect_; }
    void setFarPlaneDist(float dist);
    auto farPlaneDist()const{ return far_plane_dist_; }
    void setNearPlaneDist(float dist);
    auto nearPlaneDist()const{ return near_plane_dist_; }
    void setFovy(float f);
    auto fovy()const{ return fovy_; }
    void recalc();
    void syncDataToShader(glmShaderProgramPtr prog);
    Camera();
    ~Camera();

private:
    void calcProjection();
    void calcView();
    glmMatrix model_;
    glmMatrix view_;
    glmMatrix projection_;
    glmPt3 eye_ = glmPt3(0.0f, 0.0f, 1.0f);
    glmPt3 focal_point_ = glmPt3(0.0f, 0.0f, 0.0f);
    glmNormal viewup_ = glmNormal(0.0f, 1.0f, 0.0f);
    float win_aspect_ = 1.0f;
    float near_plane_dist_ = 0.0f;
    float far_plane_dist_ = 2.0f;
    float fovy_ = 45.0f;
};

GLMESH_NAMESPACE_END

#endif