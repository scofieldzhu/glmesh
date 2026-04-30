#include "cpu_triangle_mesh.h"
/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: cpu_triangle_mesh.cpp
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
#include "cpu_triangle_mesh.h"
#include <numeric>
#include "cpu_polygon_mesh.h"

GLMESH_NAMESPACE_BEGIN

void CpuTriangleMesh::buildFromPolygonMesh(const CpuPolygonMesh& polyon_mesh)
{
    vertices = polyon_mesh.vertices;
    indices.clear();
    for(const auto& poly : polyon_mesh.polygons){
        if(poly.size() < 3){
            continue;
        }
        for(auto i = 1; i + 1 < poly.size(); ++i){
            indices.push_back(poly[0]);
            indices.push_back(poly[i]);
            indices.push_back(poly[i + 1]);
        }
    }
}

std::optional<MeshBounds> CpuTriangleMesh::calcBounds() const
{
    if(vertices.empty()){
        return std::nullopt;
    }
    MeshBounds bounds;
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = min_x;
    float max_y = max_x;
    float min_z = min_x;
    float max_z = max_x;
    for(const auto& v : vertices){
        if(v.position.x < min_x){
            min_x = v.position.x;
        }
        if(v.position.x > max_x){
            max_x = v.position.x;
        }
        if(v.position.y < min_y){
            min_y = v.position.y;
        }
        if(v.position.y > max_y){
            max_y = v.position.y;
        }
        if(v.position.z < min_z){
            min_z = v.position.z;
        }
        if(v.position.z > max_z){
            max_z = v.position.z;
        }
    }
    bounds.min_x = min_x;
    bounds.max_x = max_x;
    bounds.min_y = min_y;
    bounds.max_y = max_y;
    bounds.min_z = min_z;
    bounds.max_z = max_z;
    bounds.center.x = (min_x + max_x) / 2.0;
    bounds.center.y = (min_y + max_y) / 2.0;
    bounds.center.z = (min_z + max_z) / 2.0;
    // 再次遍历，计算从中心点到所有顶点的最大距离作为半径 (包围球), 这样可以使得计算出来的半径更准确.
    float max_dist = 0.0f;
    for(const auto& v : vertices) {
        auto dist = glm::length(v.position - bounds.center);
        if(dist > max_dist){
            max_dist = dist;
        }
    }
    bounds.radius = max_dist;
    return bounds;
}

GLMESH_NAMESPACE_END