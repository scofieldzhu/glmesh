/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: vertex_array.cpp
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
#include "vertex_array.h"

GLMESH_NAMESPACE_BEGIN

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id_);
}

VertexArray::~VertexArray()
{
	if(id_){
		glDeleteVertexArrays(1, &id_);
	}
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
	if(this != &other){
		if(id_){
			glDeleteVertexArrays (1, &id_);
		}
		id_ = std::exchange (other.id_, 0);
	}
	return *this;
}

VertexArray::VertexArray(VertexArray&& other) noexcept
	:id_(std::exchange(other.id_, 0))
{

}

void VertexArray::bind() const noexcept
{
	glBindVertexArray(id_);
}

GLMESH_NAMESPACE_END