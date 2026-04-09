/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: trackball.cpp 
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

#include "trackball.h"
#include <spdlog/spdlog.h>
#include "mesh_renderer.h"
#include "camera.h"

GLMESH_NAMESPACE_BEGIN

Trackball::Trackball(glmMeshRendererPtr ren)
    :renderer_(ren)
{
}

Trackball::~Trackball()
{
}

glm::quat Trackball::rotate(const glm::vec2& start_pos, const glm::vec2& end_pos) const
{
    if(start_pos == end_pos){
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
    glm::vec3 start = mapToSphere(start_pos);
    glm::vec3 end = mapToSphere(end_pos);
    glm::vec3 axis = glm::cross(start, end);
    float angle = std::acos(std::min(1.0f, glm::dot(start, end)));
    return glm::angleAxis(angle, glm::normalize(axis));
}

glm::vec3 Trackball::mapToSphere(const glm::vec2& win_pos) const
{
    float x = (2.0f * win_pos.x - (float)width_) / (float)width_;
    float y = (height_ - 2.0f * win_pos.y) / (float)height_;
    float len_squared = x * x + y * y;
    if(len_squared <= 1.0f){ //lies inside of sphere 
        return glm::vec3(x, y, sqrt(1.0f - len_squared));
    }else{
        float length = std::sqrt(len_squared);
        return glm::vec3(x / length, y / length, 0.0f);
    }
}

void Trackball::handleMouseMove(const WinEvent& event)
{
    //spdlog::debug("handleMouseMove! mouse_pos:[{}, {}].", event.pos.x, event.pos.y);
    if(rotation_button_ != MouseButton::kNone && current_pressed_button_ == rotation_button_){
        glm::vec2 now_pos = event.pos;
        auto quad = rotate(tracking_mouse_pos_, now_pos);
        auto mat = renderer_->activeCamera()->model();
        mat = mat * glm::toMat4(quad);
        renderer_->activeCamera()->setModel(mat);
        tracking_mouse_pos_ = now_pos;
    }
}

void Trackball::handleMouseEvent(const WinEvent& event)
{
    if(event.type == EventType::kPress){
        current_pressed_button_ = (MouseButton)event.event_button_id;
        tracking_mouse_pos_ = event.pos;
    }else if(event.type == EventType::kRelease){
        current_pressed_button_ = MouseButton::kNone;
    }
    switch(event.event_button_id){
        case (int)MouseButton::kMiddle:
            if(event.type == EventType::kWheelScroll){
                handleWheelScroll(event);
                break;
            }
            break;

        case (int)MouseButton::kNone:
            if(event.type == EventType::kMove){
                handleMouseMove(event);
                break;
            }
            break;
        
        default:
            break;
    }
}

void Trackball::handleKeyboardEvent(const WinEvent& event)
{
}

void Trackball::handleWheelScroll(const WinEvent& event)
{
    float fv = renderer_->activeCamera()->fovy();
    fv -= event.scroll_delta;
    fv = std::max(1.0f, std::min(fv, 45.0f));
    renderer_->activeCamera()->setFovy(fv);
}

void Trackball::handleWindowEvent(const WinEvent& event)
{
    if(event.type == EventType::kResize){
        handleResize(event);
        return;
    }
}

void Trackball::handleResize(const WinEvent& event)
{
    width_ = event.win_size.x;
    height_ = event.win_size.y;
}

void Trackball::bindRotationToMouseButton(MouseButton button)
{
    rotation_button_ = button;
}

void Trackball::handleEvent(const WinEvent &event)
{
    if(event.source == EventSource::kMouseDevice){
        handleMouseEvent(event);
        return;
    }
    if(event.source == EventSource::kKeyboard){
        handleKeyboardEvent(event);
        return;
    }
    if(event.source == EventSource::kWindow){
        handleWindowEvent(event);
        return;
    }
    spdlog::warn("Invalid event source:{} identified!", (int)event.source);
}

GLMESH_NAMESPACE_END