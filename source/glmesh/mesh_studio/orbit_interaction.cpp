/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: orbit_interaction.cpp
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
#include "orbit_interaction.h"
#include "arcball_rotator.h"
#include "glmesh/kernel/gl/gl_trackball_gizmo.h"
#include "glmesh/render/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>

void OrbitInteraction::onMousePress(QMouseEvent* event, const MouseInteractionContext& ctx)
{
    if(!ctx.ball_rotator) {
        return;
    }
    if(event->buttons() & Qt::LeftButton){
        QSize sz(ctx.widget_width, ctx.widget_height);
        ctx.ball_rotator->onStartRotationEvent(event, sz);
    }
}

void OrbitInteraction::onMouseMove(QMouseEvent* event, const MouseInteractionContext& ctx)
{
    if(event->buttons() & Qt::LeftButton){
        if(!ctx.ball_rotator || !ctx.camera) {
            return;
        }
        QSize sz(ctx.widget_width, ctx.widget_height);
        ctx.ball_rotator->onUpdateMousePos(event, sz);
        ctx.camera->setRotation(ctx.ball_rotator->getRotationMat());
        if(ctx.requestUpdate) {
            ctx.requestUpdate();
        }
        return;
    }

    int new_axis = pickGizmoAxis(event->pos(), ctx);
    if(ctx.hovered_gizmo_axis && new_axis != *ctx.hovered_gizmo_axis){
        *ctx.hovered_gizmo_axis = new_axis;
        if(ctx.requestUpdate) {
            ctx.requestUpdate();
        }
    }
}

void OrbitInteraction::onWheel(QWheelEvent* event, const MouseInteractionContext& ctx)
{
    if(!ctx.camera) {
        return;
    }
    ctx.camera->zoomByWheelDelta(event->angleDelta().y());
    if(ctx.requestUpdate) {
        ctx.requestUpdate();
    }
}

float OrbitInteraction::computeGizmoWorldRadius(const MouseInteractionContext& ctx) const
{
    int h = ctx.widget_height;
    if(h <= 0 || !ctx.camera) {
        return 0.0f;
    }
    int w = ctx.widget_width;
    float pixel_radius = 0.4f * static_cast<float>(std::min(w, h));

    glm::mat4 proj = ctx.camera->projectionMatrix();
    float p11 = proj[1][1];
    if(std::abs(p11) < 1e-6f) {
        return 1.0f;
    }
    if(ctx.camera->projectionType() == glmesh::Camera::ProjectionType::Perspective) {
        float depth = ctx.camera->distance();
        return pixel_radius * (2.0f * depth) / (static_cast<float>(h) * p11);
    }
    return pixel_radius * 2.0f / (static_cast<float>(h) * p11);
}

int OrbitInteraction::pickGizmoAxis(const QPoint& pos, const MouseInteractionContext& ctx) const
{
    if(!ctx.gizmo || !ctx.gizmo->valid()) {
        return -1;
    }
    int w = ctx.widget_width;
    int h = ctx.widget_height;
    if(w <= 0 || h <= 0) {
        return -1;
    }
    // computeGizmoWorldRadius is non-const because pick is conceptually read-only,
    // we cast away const on 'this' to match the original pattern.
    float world_radius = const_cast<OrbitInteraction*>(this)->computeGizmoWorldRadius(ctx);
    if(world_radius <= 0.0f) {
        return -1;
    }

    glm::mat4 mvp = ctx.camera->projectionMatrix() *
                    ctx.camera->viewMatrix() *
                    glm::scale(glm::mat4(1.0f), glm::vec3(world_radius));

    auto project = [&](const glm::vec3& wp, glm::vec2& out_screen) -> bool {
        glm::vec4 clip = mvp * glm::vec4(wp, 1.0f);
        if(clip.w <= 0.0f) {
            return false;
        }
        glm::vec3 ndc(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
        out_screen.x = (ndc.x * 0.5f + 0.5f) * static_cast<float>(w);
        out_screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(h);
        return true;
    };

    const float pick_threshold_px = 8.0f;
    glm::vec2 mouse(static_cast<float>(pos.x()), static_cast<float>(pos.y()));
    int seg = ctx.gizmo->segments();
    int best_axis = -1;
    float best_d2 = pick_threshold_px * pick_threshold_px;

    for(int axis = 0; axis < 3; ++axis) {
        for(int i = 0; i < seg; ++i) {
            glm::vec3 a_w = glmesh::GLTrackballGizmo::axisRingPoint(axis, i, seg);
            glm::vec3 b_w = glmesh::GLTrackballGizmo::axisRingPoint(axis, (i + 1) % seg, seg);
            glm::vec2 a_s, b_s;
            if(!project(a_w, a_s) || !project(b_w, b_s)) {
                continue;
            }
            glm::vec2 ab = b_s - a_s;
            float ab_len2 = glm::dot(ab, ab);
            float t = ab_len2 > 1e-6f ? glm::clamp(glm::dot(mouse - a_s, ab) / ab_len2, 0.0f, 1.0f) : 0.0f;
            glm::vec2 closest = a_s + t * ab;
            float d2 = glm::dot(mouse - closest, mouse - closest);
            if(d2 < best_d2) {
                best_d2 = d2;
                best_axis = axis;
            }
        }
    }
    return best_axis;
}