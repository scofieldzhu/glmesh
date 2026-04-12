/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: vertex_buffer.h 
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
#include "index_buffer.h"
#include "glad.h"

GLMESH_NAMESPACE_BEGIN

IndexBuffer::IndexBuffer()
{
	glGenBuffers(1, &id_);
}

IndexBuffer::~IndexBuffer()
{
	if(id_){
		glDeleteBuffers(1, &id_);
	}
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
	if(this != &other){
		if(id_){ 
			glDeleteBuffers(1, &id_);
		}
		id_ = std::exchange(other.id_, 0);
	}
	return *this;
}

void IndexBuffer::bind() const noexcept
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_);
}

void IndexBuffer::upload(const void* data, std::size_t size, uint32_t usage /*= GL_STATIC_DRAW*/) const
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, usage);
}

GLMESH_NAMESPACE_END