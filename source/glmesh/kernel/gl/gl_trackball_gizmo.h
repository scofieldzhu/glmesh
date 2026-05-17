/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: gl_trackball_gizmo.h
 *  Copyright (c) 2024-2026 scofieldzhu
 *
 *  MIT License
 */
#ifndef __gl_trackball_gizmo_h__
#define __gl_trackball_gizmo_h__

#include "glmesh/kernel/gl/vertex_array.h"
#include "glmesh/kernel/gl/vertex_buffer.h"
#include "glmesh/kernel/gl/gl_drawable.h"

GLMESH_NAMESPACE_BEGIN

class GLMESH_KERNEL_API GLTrackballGizmo : public GLDrawable
{
public:
    void create(int segments = 96);
    void draw() const override;
    void drawAxis(int axis) const;
    bool valid() const { return created_; }
    int segments() const { return vertex_count_; }
    // axis: 0=X(YZ-plane ring), 1=Y(XZ-plane ring), 2=Z(XY-plane ring)
    // i: vertex index in [0, segments)
    static glm::vec3 axisRingPoint(int axis, int i, int segments);

private:
    VertexArray vao_[3];
    VertexBuffer vbo_[3];
    int vertex_count_ = 0;
    bool created_ = false;
};

GLMESH_NAMESPACE_END

#endif
