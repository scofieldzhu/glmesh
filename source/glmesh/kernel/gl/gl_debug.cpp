/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: gl_debug.cpp
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
#include "gl_debug.h"
#include "glad/glad.h"
#include <cstdio>

GLMESH_NAMESPACE_BEGIN

namespace gl
{
    namespace
    {
        const char* sourceToString(GLenum source) noexcept
        {
            switch (source)
            {
            case GL_DEBUG_SOURCE_API:             return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WindowSystem";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "ShaderCompiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:     return "ThirdParty";
            case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
            case GL_DEBUG_SOURCE_OTHER:           return "Other";
            default:                              return "UnknownSource";
            }
        }

        const char* typeToString(GLenum type) noexcept
        {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:               return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DeprecatedBehavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UndefinedBehavior";
            case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
            case GL_DEBUG_TYPE_MARKER:              return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP:          return "PushGroup";
            case GL_DEBUG_TYPE_POP_GROUP:           return "PopGroup";
            case GL_DEBUG_TYPE_OTHER:               return "Other";
            default:                                return "UnknownType";
            }
        }

        const char* severityToString(GLenum severity) noexcept
        {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:         return "High";
            case GL_DEBUG_SEVERITY_MEDIUM:       return "Medium";
            case GL_DEBUG_SEVERITY_LOW:          return "Low";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
            default:                             return "UnknownSeverity";
            }
        }

        const char* errorToString(GLenum err) noexcept
        {
            switch (err)
            {
            case GL_NO_ERROR:                      return "GL_NO_ERROR";
            case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
            case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
            case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
            case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
            case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
            default:                               return "UnknownGLerror";
            }
        }

        bool hasDebugSupport() noexcept
        {
            return GLAD_GL_VERSION_4_3;
        }

        void APIENTRY debugCallback(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei /*length*/,
                                    const GLchar* message,
                                    const void* /*userParam*/)
        {
            std::fprintf(stderr,
                         "[OpenGL][%s][%s][%s][id=%u] %s\n",
                         sourceToString(source),
                         typeToString(type),
                         severityToString(severity),
                         id,
                         message ? message : "");
        }
    }

    void EnableDebugOutput(bool synchronous, bool enableNotifications) 
    {
        if (!hasDebugSupport())
        {
            std::fprintf(stderr,
                         "[OpenGL] debug output is not available "
                         "(requires OpenGL 4.3 or KHR_debug).\n");
            return;
        }

        glEnable(GL_DEBUG_OUTPUT);

        if (synchronous)
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        else
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(debugCallback, nullptr);

        if (!enableNotifications)
        {
            glDebugMessageControl(GL_DONT_CARE,
                                  GL_DONT_CARE,
                                  GL_DEBUG_SEVERITY_NOTIFICATION,
                                  0,
                                  nullptr,
                                  GL_FALSE);
        }
    }

    void DisableDebugOutput()
    {
        if (!hasDebugSupport())
            return;

        glDebugMessageCallback(nullptr, nullptr);
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDisable(GL_DEBUG_OUTPUT);
    }

    bool CheckGlError(const char* expr, const char* file, int line)
    {
        bool ok = true;

        for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError())
        {
            ok = false;
            std::fprintf(stderr,
                         "[OpenGL][glGetError] %s after `%s` at %s:%d\n",
                         errorToString(err),
                         expr ? expr : "",
                         file ? file : "",
                         line);
        }

        return ok;
    }
}

GLMESH_NAMESPACE_END