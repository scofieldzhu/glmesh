/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: actor.cpp 
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
#include "actor.h"
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

GLMESH_NAMESPACE_BEGIN

Actor::Actor()
    :matrix_(glm::identity<glm::mat4>())
{
}

Actor::~Actor()
{    
}

bool Actor::addToRenderer(glmMeshRendererPtr ren)
{
    if(ren == nullptr){
        spdlog::error("Null renderer pointer!");
        return false;
    }
    if(!source_created_){
        if(!createSource(ren.get())){
            spdlog::error("Create source failed!");
            return false;
        }
        source_created_ = true;        
    }
    renderers_.push_back(ren);
    return true;
}

bool Actor::existRenderer() const
{
    return !renderers_.empty();
}

void Actor::removeFromRenderer(glmMeshRendererPtr ren)
{
}

GLMESH_NAMESPACE_END