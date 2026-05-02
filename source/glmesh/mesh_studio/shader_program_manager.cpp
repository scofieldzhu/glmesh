/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: shader_program_manager.cpp
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
#include "shader_program_manager.h"
#include "app_log.h"

void ShaderProgramManager::addProgram(ProgramTypeId type_id, std::unique_ptr<glmesh::ShaderProgram> pro)
{
    if(getProgram(type_id)){
        APP_LOG_WARN("type_id:{} already exists!", type_id);
        return;
    }
    program_dict_.insert({type_id, std::move(pro)});
}

glmesh::ShaderProgram* ShaderProgramManager::getProgram(ProgramTypeId type_id) const
{
    if(!program_dict_.contains(type_id)){
        return nullptr;
    }
    return program_dict_.at(type_id).get();
}

void ShaderProgramManager::destory()
{
    for(const auto& kv : program_dict_){
        kv.second->destroy();
    }
    program_dict_.clear();
}
