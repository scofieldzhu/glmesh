/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: geometry_utils.cpp
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
#include "geometry_utils.h"

GLMESH_NAMESPACE_BEGIN

// std::optional<glm::vec3>
// RayTriangleIntersect(const Ray& ray, 
//                      const glm::vec3& v0, 
//                      const glm::vec3& v1, const glm::vec3& v2, 
//                      float& t)
// {
//     const float EPSILON = 1e-6f;
//     glm::vec3 edge1 = v1 - v0;
//     glm::vec3 edge2 = v2 - v0;
//     glm::vec3 h = glm::cross(ray.direction, edge2);
//     // a = e1•(D × e2) <=> a = D•(e1 × e2)
//     float a = glm::dot(edge1, h);
//     if(a > -EPSILON && a < EPSILON){
//         // 射线与三角形平行
//         return std::nullopt; 
//     } 
//     float f = 1.0f / a;
//     glm::vec3 s = ray.origin - v0;
//     float u = f * glm::dot(s, h);
//     if(u < 0.0f || u > 1.0f){
//         return std::nullopt; 
//     }
//     glm::vec3 q = glm::cross(s, edge1);
//     float v = f * glm::dot(ray.direction, q);
//     if(v < 0.0f || u + v > 1.0f){
//         return std::nullopt;    
//     } 
//     t = f * glm::dot(edge2, q);
//     if(t > EPSILON){ // 射线正方向相交
//         return ray.origin + ray.direction * t;
//     }
//     return std::nullopt;
// }

GLMESH_NAMESPACE_END