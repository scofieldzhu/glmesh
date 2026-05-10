/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: camera.h 
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
#ifndef __camera_h__
#define __camera_h__

#include "glmesh/render/glmesh_render_typedef.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

GLMESH_NAMESPACE_BEGIN

class GLMESH_RENDER_API Camera
{
public:
    enum class ProjectionType{
        Perspective,
        Orthographic
    };
    void setProjectionType(ProjectionType type);
    ProjectionType projectionType() const{ return projection_type_; }
    void reset();
    void setViewport(int width, int height);
    void setPerspective(float fov_y_deg, float near_plane, float far_plane);
    void setOrthographicScale(float scale);
    void setCenter(const glm::vec3& center);
    const glm::vec3& center() const{ return center_; }
    void setCenterOffset(const glm::vec3& center_offset);
    const glm::vec3& centerOffset() const{ return center_offset_; }
    void setDistance(float distance);
    float distance() const{ return distance_; }
    void setRotation(const glm::mat4& rotation);
    void setRotation(const glm::quat& rotation);
    const glm::quat& rotation() const{ return rotation_; }
    void fitBounds(const glm::vec3& min_bound, const glm::vec3& max_bound);
    void fitBounds(const glm::vec3& center, float radius);
    void zoomByWheelDelta(float wheel_delta);
    void zoom(float factor);
    void pan(float delta_x, float delta_y);
    void orbit(float delta_x, float delta_y);
    glm::mat4 viewMatrix() const;
    glm::mat4 viewMatrix(const glm::mat4& external_rotation) const;
    glm::mat4 projectionMatrix() const;
    glm::mat4 modelCenterMatrix() const;
    glm::vec3 position() const;
    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::vec3 up() const;
    float aspectRatio() const;
    Camera();
    ~Camera() = default;

private:
    float clampDistance(float distance) const;
    ProjectionType projection_type_ = ProjectionType::Perspective;
    int viewport_width_ = 1;
    int viewport_height_ = 1;
    glm::vec3 center_ = {0.0f, 0.0f, 0.0f};
    glm::vec3 center_offset_ = {0.0f, 0.0f, 0.0f};
    float distance_ = 100.0f;
    float min_distance_ = 1.0f;
    float max_distance_ = 1000.0f;
    glm::quat rotation_ = {1.0f, 0.0f, 0.0f, 0.0f};
    float fov_y_deg_ = 45.0f;
    float near_plane_ = 0.1f;
    float far_plane_ = 1000.0f;
    float orthographic_scale_ = 10.0f;
    float orbit_sensitivity_ = 0.005f;
    float pan_sensitivity_ = 0.0015f;
};

GLMESH_NAMESPACE_END

#endif