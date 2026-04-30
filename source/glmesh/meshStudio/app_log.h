/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: log.h
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
#ifndef __app_log_h__
#define __app_log_h__

#include <spdlog/spdlog.h>

void InitLogger();
std::shared_ptr<spdlog::logger> GetAppLogger();

#define APP_LOG_TRACE(...) SPDLOG_LOGGER_TRACE(GetAppLogger(), __VA_ARGS__)
#define APP_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(GetAppLogger(), __VA_ARGS__)
#define APP_LOG_INFO(...) SPDLOG_LOGGER_INFO(GetAppLogger(), __VA_ARGS__)
#define APP_LOG_WARN(...) SPDLOG_LOGGER_WARN(GetAppLogger(), __VA_ARGS__)
#define APP_LOG_ERROR(...) SPDLOG_LOGGER_ERROR(GetAppLogger(), __VA_ARGS__)
#define APP_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(GetAppLogger(), __VA_ARGS__)

#define APP_ASSERT(cond, ...) \
    if(!(cond)){ \
        APP_LOG_CRITICAL(__VA_ARGS__) \
    }

#endif