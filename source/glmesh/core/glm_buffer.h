/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: glm_buffer.h 
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

#ifndef __glm_buffer_h__
#define __glm_buffer_h__

#include "glmesh/core/glm_base_type.h"
#include "glmesh/core/glm_export.h"
#include "glmesh/core/glm_instantiator.h"
#include <optional>

GLMESH_NAMESPACE_BEGIN

class GLMESH_API glmBuffer : public glmInstantiator<glmBuffer>
{
public:
    auto type()const{ return type_; }
    bool valid()const{ return id_.has_value(); }
    auto id()const{ return id_.value(); }
    bool createImmutableDataStore(uint32_t size, const void* data, uint32_t flags);    
    auto dataStoreCreated()const{ return data_store_created_; }
    bool writeSubData(int32_t offset, uint32_t size, const void* data);
    glmBuffer(uint32_t type);
    ~glmBuffer();

private:
    std::optional<uint32_t> id_;
    bool data_store_created_ = false;
    uint32_t type_;
};

GLMESH_NAMESPACE_END

#endif