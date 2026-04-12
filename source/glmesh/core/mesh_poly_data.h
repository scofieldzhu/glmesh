/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_poly_data.h 
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

#ifndef __mesh_poly_data_h__
#define __mesh_poly_data_h__

#include "glmesh/core/memory_block.h"
#include "glmesh/core/glm_export.h"
#include "glmesh/core/instantiator.h"
#include "glmesh/core/size_eval.hpp"

GLMESH_NAMESPACE_BEGIN

struct GLMESH_API MeshPolyData : public Instantiator<MeshPolyData>
{
    static constexpr glmIndex kPolyRestartIndex = 0xFFFFFFFF;    
    static constexpr size_t kMaxVertexNumber = 50e4;
    static constexpr size_t kMaxFacetNumber = 50e4;
    static constexpr size_t kMaxVertexNumberOfPolyFacet = 6;
    static constexpr size_t kMaxFacetByteSize = kMaxFacetNumber * (kMaxVertexNumberOfPolyFacet + 1) * kIndexTypeSize;
    static constexpr size_t kMaxVertexRelatedByteSize = (kVertexTypeSize + kColorTypeSize + kNormalTypeSize) * kMaxVertexNumber;
    //static constexpr size_t kMaxMeshByteSize = kMaxVertexRelatedByteSize + kMaxFacetByteSize;
    bool valid()const;
    glmMemoryBlockPtr allocFacetData();
    glmBoundingBox calcBoundingBox()const;
    glm::vec3 calcCenterPoint()const;
    uint32_t calcIndiceCount()const;
    uint32_t calcByteSizeOfFacets()const{ return calcIndiceCount() * kIndexTypeSize; }
    bool triangulated()const{ return !triangle_facets.empty(); }
    bool existFacetData()const{ return triangle_facets.size() || poly_facets.size(); }
    glmVertexList vertices;
    glmColorList colors;
    glmNormalList normals;
    glmTriangleFacetSet triangle_facets;
    glmPolyFacetList poly_facets;
};

GLMESH_NAMESPACE_END

#endif