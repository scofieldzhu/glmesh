/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: glmesh_log.h 
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
#ifndef __glmesh_log_h__
#define __glmesh_log_h__

#include "glmesh_kernel_export.h"
#include <spdlog/spdlog.h>

GLMESH_NAMESPACE_BEGIN

namespace logging{
    GLMESH_KERNEL_API void InitLogger(std::shared_ptr<spdlog::logger> logger);
    GLMESH_KERNEL_API std::shared_ptr<spdlog::logger> GetLogger();
}

#define GLMESH_LOG_TRACE(...) SPDLOG_LOGGER_TRACE(::glmesh::logging::GetLogger(), __VA_ARGS__)
#define GLMESH_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(::glmesh::logging::GetLogger(), __VA_ARGS__)
#define GLMESH_LOG_INFO(...) SPDLOG_LOGGER_INFO(::glmesh::logging::GetLogger(), __VA_ARGS__)
#define GLMESH_LOG_WARN(...) SPDLOG_LOGGER_WARN(::glmesh::logging::GetLogger(), __VA_ARGS__)
#define GLMESH_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(::glmesh::logging::GetLogger(), __VA_ARGS__)
#define GLMESH_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::glmesh::logging::GetLogger(), __VA_ARGS__)

#define GLMESH_ASSERT(cond, ...) \
    if(!(cond)){ \
        GLMESH_LOG_CRITICAL(__VA_ARGS__) \
    }

GLMESH_NAMESPACE_END

#endif