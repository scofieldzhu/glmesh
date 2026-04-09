/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: bkg_actor.h 
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

#ifndef __bkg_actor_h__
#define __bkg_actor_h__

#include "glmesh/core/actor.h"
#include "glmesh/core/instantiator.h"

GLMESH_NAMESPACE_BEGIN

class glmBkgActor : public Actor, public Instantiator<glmBkgActor>
{
public:    
    void draw(MeshRenderer* ren) override;
    void setTopColor(const glm::vec3& clr);
    void setBottomColor(const glm::vec3& clr);
    glmBkgActor();
    ~glmBkgActor();

private:
    bool createSource(MeshRenderer* ren) override;
    glm::vec3 top_color_ = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 bottom_color_ = glm::vec3(0.5f, 0.5f, 1.0f);
    glmShaderProgramPtr program_;
    glmVertexArrayPtr vao_;
    glmBufferPtr vertex_buffer_;
};

GLMESH_NAMESPACE_END

#endif // __bkg_actor_h__