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
#include "camera.h"
#include <algorithm>
#include <cmath>

GLMESH_NAMESPACE_BEGIN

Camera::Camera()
{
    reset();
}

void Camera::reset()
{
    projection_type_ = ProjectionType::Perspective;

    viewport_width_ = 1;
    viewport_height_ = 1;

    center_ = glm::vec3(0.0f, 0.0f, 0.0f);
    center_offset_ = glm::vec3(0.0f, 0.0f, 0.0f);

    distance_ = 100.0f;
    min_distance_ = 1.0f;
    max_distance_ = 1000.0f;

    rotation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    fov_y_deg_ = 45.0f;
    near_plane_ = 0.1f;
    far_plane_ = 1000.0f;

    orthographic_scale_ = 10.0f;
}

void Camera::setViewport(int width, int height)
{
    viewport_width_ = std::max(1, width);
    viewport_height_ = std::max(1, height);
}

void Camera::setProjectionType(ProjectionType type)
{
    projection_type_ = type;
}

void Camera::setPerspective(float fov_y_deg, float near_plane, float far_plane)
{
    fov_y_deg_ = std::clamp(fov_y_deg, 1.0f, 120.0f);
    near_plane_ = std::max(0.0001f, near_plane);
    far_plane_ = std::max(near_plane_ + 0.001f, far_plane);
}

void Camera::setOrthographicScale(float scale)
{
    orthographic_scale_ = std::max(0.0001f, scale);
}

void Camera::setCenter(const glm::vec3& center)
{
    center_ = center;
}

void Camera::setCenterOffset(const glm::vec3& center_offset)
{
    center_offset_ = center_offset;
}

void Camera::setDistance(float distance)
{
    distance_ = clampDistance(distance);
}

void Camera::setRotation(const glm::mat4& rotation)
{
    rotation_ = glm::quat_cast(rotation);
}

void Camera::setRotation(const glm::quat& rotation)
{
    rotation_ = glm::normalize(rotation);
}

void Camera::fitBounds(const glm::vec3& min_bound, const glm::vec3& max_bound)
{
    glm::vec3 center = (min_bound + max_bound) * 0.5f;
    float radius = glm::length(max_bound - min_bound) * 0.5f;
    fitBounds(center, radius);
}

void Camera::fitBounds(const glm::vec3& center, float radius)
{
    if(radius <= 0.000001f){
        radius = 1.0f;
    }
    center_ = center;
    center_offset_ = -center;
    distance_ = radius * 2.0f;
    min_distance_ = radius * 1.2f;
    max_distance_ = radius * 10.0f;
    near_plane_ = std::max(radius * 0.01f, 0.001f);
    far_plane_ = radius * 20.0f;
    orthographic_scale_ = radius * 2.0f;
}

void Camera::zoomByWheelDelta(float wheel_delta)
{
    float scroll_amount = wheel_delta / 120.0f;
    float step = (max_distance_ - min_distance_) * 0.05f;
    distance_ -= scroll_amount * step;
    distance_ = clampDistance(distance_);
}

void Camera::zoom(float factor)
{
    factor = std::clamp(factor, 0.05f, 20.0f);
    distance_ = clampDistance(distance_ * factor);
    if(projection_type_ == ProjectionType::Orthographic) {
        orthographic_scale_ = std::max(0.0001f, orthographic_scale_ * factor);
    }
}

void Camera::pan(float delta_x, float delta_y)
{
    float scale = distance_ * pan_sensitivity_;
    glm::vec3 offset = -right() * delta_x * scale + up() * delta_y * scale;
    center_ += offset;
    center_offset_ = -center_;
}

void Camera::orbit(float delta_x, float delta_y)
{
    float yaw_angle = -delta_x * orbit_sensitivity_;
    float pitch_angle = -delta_y * orbit_sensitivity_;
    glm::quat yaw = glm::angleAxis(yaw_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat pitch = glm::angleAxis(pitch_angle, right());
    rotation_ = glm::normalize(yaw * pitch * rotation_);
}

glm::mat4 Camera::viewMatrix() const
{
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -distance_));
    view = view * glm::toMat4(rotation_);
    return view;
}

glm::mat4 Camera::viewMatrix(const glm::mat4& external_rotation) const
{
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -distance_));
    view = view * external_rotation;
    return view;
}

glm::mat4 Camera::projectionMatrix() const
{
    float aspect = aspectRatio();
    if(projection_type_ == ProjectionType::Perspective) {
        return glm::perspective(glm::radians(fov_y_deg_), aspect, near_plane_, far_plane_);
    }
    float half_height = orthographic_scale_ * 0.5f;
    float half_width = half_height * aspect;
    return glm::ortho(-half_width, half_width, -half_height, half_height, near_plane_, far_plane_);
}

glm::mat4 Camera::modelCenterMatrix() const
{
    return glm::translate(glm::mat4(1.0f), center_offset_);
}

glm::vec3 Camera::position() const
{
    return center_ - forward() * distance_;
}

glm::vec3 Camera::forward() const
{
    return glm::normalize(rotation_ * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Camera::right() const
{
    return glm::normalize(rotation_ * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Camera::up() const
{
    return glm::normalize(rotation_ * glm::vec3(0.0f, 1.0f, 0.0f));
}

float Camera::aspectRatio() const
{
    return static_cast<float>(viewport_width_) / static_cast<float>(viewport_height_);
}

float Camera::clampDistance(float distance) const
{
    return std::clamp(distance, min_distance_, max_distance_);
}

GLMESH_NAMESPACE_END