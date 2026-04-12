/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: renderer.h
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
#include "renderer.h"
#include "gl_triangle_mesh.h"
#include "material.h"
#include "shader_program.h"

GLMESH_NAMESPACE_BEGIN

void DrawMesh(const GLTriangleMesh &mesh, 
              const Material &material, 
              const glm::mat4 &model, 
              const glm::mat4 &view, 
              const glm::mat4 &proj
            )
{
    material.bind();

    material.shader->setMat4("uModel", model);
    material.shader->setMat4("uView", view);
    material.shader->setMat4("uProj", proj);

    mesh.draw();
}

// app loop:
// ShaderProgram meshShader;
// meshShader.createFromFiles("mesh.vert", "mesh.frag");

// Material material;
// material.shader = &meshShader;
// material.base_color = glm::vec3(1.0f, 1.0f, 1.0f);
// material.light_dir  = glm::vec3(1.0f, -1.0f, -1.0f);
// material.ambient    = 0.2f;
// material.diffuse    = 1.0f;

// GLTriangleMesh mesh = loadPlyRenderableMesh("bunny.ply");


//render:
// DrawMesh(mesh, material, model, view, proj);

GLMESH_NAMESPACE_END
