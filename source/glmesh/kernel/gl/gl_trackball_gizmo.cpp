/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gl_trackball_gizmo.cpp
 *  Copyright (c) 2024-2026 scofieldzhu
 *
 *  MIT License
 */
#include "gl_trackball_gizmo.h"
#include <vector>
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include "glad/glad.h"

GLMESH_NAMESPACE_BEGIN

glm::vec3 GLTrackballGizmo::axisRingPoint(int axis, int i, int segments)
{
    const float two_pi = 2 * glm::pi<float>();
    float t = static_cast<float>(i) * (two_pi / static_cast<float>(segments));
    float c = std::cos(t);
    float s = std::sin(t);
    switch(axis){
        case 0: 
            return glm::vec3(0.0f, c, s);   // X axis ring (YZ plane)
        case 1: 
            return glm::vec3(c, 0.0f, s);   // Y axis ring (XZ plane)
        default: 
            return glm::vec3(c, s, 0.0f);  // Z axis ring (XY plane)
    }
}

void GLTrackballGizmo::create(int segments)
{
    if(segments < 8){
        segments = 8;
    }
    vertex_count_ = segments;

    std::vector<glm::vec3> rings[3];
    for(int axis_id = 0; axis_id < 3; ++axis_id){
        rings[axis_id].resize(segments);
        for(int seg_id = 0; seg_id < segments; ++seg_id){
            rings[axis_id][seg_id] = axisRingPoint(axis_id, seg_id, segments);
        }
    }

    auto upload_ring = [&](int axis_id, const std::vector<glm::vec3>& pts){
        vao_[axis_id].bind();
        vbo_[axis_id].upload(pts.data(), pts.size() * sizeof(glm::vec3), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        vao_[axis_id].unbind();
    };

    upload_ring(0, rings[0]);
    upload_ring(1, rings[1]);
    upload_ring(2, rings[2]);

    created_ = true;
}

void GLTrackballGizmo::drawAxis(int axis) const
{
    if(!created_ || axis < 0 || axis > 2){
        return;
    }
    vao_[axis].bind();
    glDrawArrays(GL_LINE_LOOP, 0, vertex_count_);
}

void GLTrackballGizmo::draw() const
{
    if(!created_){
        return;
    }
    for(int i = 0; i < 3; ++i){
        vao_[i].bind();
        glDrawArrays(GL_LINE_LOOP, 0, vertex_count_);
    }
}

GLMESH_NAMESPACE_END
