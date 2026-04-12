/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_poly_data.cpp 
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

#include "mesh_poly_data.h"
#include <spdlog/spdlog.h>

GLMESH_NAMESPACE_BEGIN

glmBoundingBox MeshPolyData::calcBoundingBox() const
{
    glmBoundingBox box;
    for(const auto& v : vertices) {
        box.min.x = std::min(box.min.x, v.x);
        box.min.y = std::min(box.min.y, v.y);
        box.min.z = std::min(box.min.z, v.z);
        box.max.x = std::max(box.max.x, v.x);
        box.max.y = std::max(box.max.y, v.y);
        box.max.z = std::max(box.max.z, v.z);
    }
    return box;
}

glm::vec3 MeshPolyData::calcCenterPoint() const
{
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    for(const auto& v : vertices){
        center[0] += v[0];
        center[1] += v[1];
        center[2] += v[2];
    }
    return  center * (1.0f / vertices.size());
}

uint32_t MeshPolyData::calcIndiceCount() const
{
    if(!triangle_facets.empty()){
        return triangle_facets.size() * 3;
    }
    std::size_t count = 0;
    if(!poly_facets.empty()){
        for(const auto& pf : poly_facets){
            count += pf.size();
        }
        count += (poly_facets.size() - 1);
    }
    return count;
}

bool MeshPolyData::valid() const
{
    if(vertices.empty() || vertices.size() > kMaxVertexNumber){
        return false;
    }
    return calcByteSizeOfFacets() < MeshPolyData::kMaxFacetByteSize;
}

glmMemoryBlockPtr MeshPolyData::allocFacetData()
{
    if(poly_facets.empty()){
        if(triangle_facets.empty()){
            spdlog::error("No facets data found!");
            return nullptr;            
        }
        char* data_ptr = reinterpret_cast<char*>(triangle_facets.data());
        size_t size = triangle_facets.size() * sizeof(glmTriangleFacet);
        return std::make_shared<MemoryBlock>(data_ptr, size);     
    }
    size_t total_size = 0;
    for(auto pf : poly_facets)
        total_size += pf.size() * kIndexTypeSize;
    total_size += kIndexTypeSize * (poly_facets.size() - 1); //restart indexes size
    auto data_block = std::make_shared<MemoryBlock>(total_size);
    auto cur_data_ptr = reinterpret_cast<glmIndex*>(data_block->blockData());
    auto end_data_ptr = data_block->blockData() + total_size;
    for(const auto& pf : poly_facets){
        memcpy(cur_data_ptr, pf.data(), pf.size() * kIndexTypeSize);
        cur_data_ptr += pf.size();
        if((char*)cur_data_ptr != end_data_ptr){
            *cur_data_ptr = kPolyRestartIndex;
            ++cur_data_ptr;
        }
    }
    return data_block;
}

GLMESH_NAMESPACE_END
