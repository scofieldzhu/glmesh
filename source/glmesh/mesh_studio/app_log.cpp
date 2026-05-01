#include "app_log.h"
/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: log.cpp
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
#include "app_log.h"
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "glmesh/kernel/glmesh_log.h"

namespace details{
    std::shared_ptr<spdlog::logger> sAppLogger;

    void InitLogger()
    {
        auto logger = spdlog::stdout_color_mt("console");
        logger->set_level(spdlog::level::trace);
        logger->set_pattern(
            "[%Y-%m-%d %H:%M:%S.%e]"
            "[%l]"
            "[%t]"
            "[%s:%!:%#]"
            "%v"
        );    
        glmesh::logging::InitLogger(logger);
        sAppLogger = logger;
    }

    std::shared_ptr<spdlog::logger> GetAppLogger()
    {
        return sAppLogger;
    }
}

std::string QStrToLogStr(const QString& qstr)
{
    if(::IsDebuggerPresent() != FALSE){
        return qstr.toLocal8Bit().toStdString();
    }
    return qstr.toUtf8().toStdString();
}