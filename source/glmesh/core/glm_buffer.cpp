/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: glm_buffer.cpp 
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

#include "glm_buffer.h"
#include "glad/glad.h"
#include <spdlog/spdlog.h>

GLMESH_NAMESPACE_BEGIN

glmBuffer::glmBuffer(uint32_t type)
    :type_(type)
{
    uint32_t buf_id = 0;
    glCreateBuffers(1, &buf_id);
    if(glGetError() == GL_NO_ERROR && buf_id != 0 && glIsBuffer(buf_id)){
        id_ = buf_id;
    }
}

glmBuffer::~glmBuffer()
{
    if(id_){
        glDeleteBuffers(1, &(id_.value()));
    }    
}

bool glmBuffer::createImmutableDataStore(uint32_t size, const void* data, uint32_t flags)
{
    if(!valid()){
        SPDLOG_ERROR("It's not valid buffer id!");
        return false;
    }
    glNamedBufferStorage(id_.value(), size, data, flags);
    GLenum err = glGetError();
    if(err == GL_NO_ERROR){
        data_store_created_ = true;
        return true;
    }
    SPDLOG_ERROR("CreateImmutableDataStore failed! GLenum err:{}", err);
    return false;
}

bool glmBuffer::writeSubData(int32_t offset, uint32_t size, const void* data)
{
    if(!valid()){
        SPDLOG_ERROR("It's not valid buffer id!");
        return false;
    }
    if(!data_store_created_){
        SPDLOG_ERROR("Data store not created yet!");
        return false;
    }
    glNamedBufferSubData(id_.value(), offset, size, data);
    GLenum err = glGetError();
    if(err == GL_NO_ERROR){
        return true;
    }
    SPDLOG_ERROR("WriteSubData failed! GLenum err:{}", err);
    return false;
}

GLMESH_NAMESPACE_END