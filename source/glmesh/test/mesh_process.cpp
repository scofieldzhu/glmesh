/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: mesh_process.cpp 
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

#include "mesh_process.h"
#include <pcl/point_types.h> 
#include <pcl/point_cloud.h> 
#include <pcl/PolygonMesh.h>
#include <pcl/surface/ear_clipping.h>
#include "glmesh/core/mesh_poly_data.h"

using namespace glmesh;

pcl::PolygonMesh::Ptr glmMeshToPclMesh(glmesh::glmMeshPtr source_mesh)
{
    pcl::PCLPointCloud2::Ptr cloud(new pcl::PCLPointCloud2);
    cloud->fields.resize(3);
    cloud->fields[0].name = "x";
    cloud->fields[0].offset = 0;
    cloud->fields[0].datatype = pcl::PCLPointField::FLOAT32;
    cloud->fields[0].count = 1;
    cloud->fields[1].name = "y";
    cloud->fields[1].offset = 4;
    cloud->fields[1].datatype = pcl::PCLPointField::FLOAT32;
    cloud->fields[1].count = 1;
    cloud->fields[2].name = "z";
    cloud->fields[2].offset = 8;
    cloud->fields[2].datatype = pcl::PCLPointField::FLOAT32;
    cloud->fields[2].count = 1;
    cloud->width = source_mesh->vertices.size();
    cloud->height = 1;
    cloud->point_step = 12;
    cloud->is_dense = true;
    cloud->row_step = cloud->point_step * cloud->width;
    cloud->data.resize(cloud->row_step * cloud->height);
    for(size_t i = 0; i < source_mesh->vertices.size(); ++i){
        float* data = reinterpret_cast<float*>(&cloud->data[i * cloud->point_step]);
        data[0] = source_mesh->vertices[i].x;
        data[1] = source_mesh->vertices[i].y;
        data[2] = source_mesh->vertices[i].z;
    }
    pcl::PolygonMesh::Ptr pcl_mesh(new pcl::PolygonMesh());
    pcl_mesh->cloud = *cloud;
    for(const auto& f : source_mesh->poly_facets){
        pcl::Vertices vertices;
        for(const auto& idx : f){
            vertices.vertices.push_back(idx);
        }
        pcl_mesh->polygons.push_back(vertices);
    }
    return pcl_mesh;
}

glmMeshPtr PclMeshToglmMesh(pcl::PolygonMesh::Ptr pcl_mesh)
{
    glmMeshPtr result_mesh = std::make_shared<MeshPolyData>();
    for(size_t i = 0; i < pcl_mesh->cloud.data.size(); i += pcl_mesh->cloud.point_step){
        float x = *reinterpret_cast<float*>(&pcl_mesh->cloud.data[i + pcl_mesh->cloud.fields[0].offset]);
        float y = *reinterpret_cast<float*>(&pcl_mesh->cloud.data[i + pcl_mesh->cloud.fields[1].offset]);
        float z = *reinterpret_cast<float*>(&pcl_mesh->cloud.data[i + pcl_mesh->cloud.fields[2].offset]);
        result_mesh->vertices.push_back({x, y, z}); 
    }
    for(const auto& polygon : pcl_mesh->polygons){
        assert(polygon.vertices.size() == 3);
        result_mesh->triangle_facets.push_back(glmTriangleFacet(polygon.vertices[0], polygon.vertices[1], polygon.vertices[2]));
    }
    return result_mesh;
}

glmMeshPtr Triangulate(glmMeshPtr poly_mesh)
{
    if(!poly_mesh->triangle_facets.empty()){
        return nullptr;
    }    
    if(poly_mesh->poly_facets.empty()){
        return nullptr;
    }
    auto pcl_mesh = glmMeshToPclMesh(poly_mesh);    
    pcl::EarClipping ear_clipping;
    ear_clipping.setInputMesh(pcl_mesh);
    pcl::PolygonMesh::Ptr output_mesh = std::make_shared<pcl::PolygonMesh>();
    ear_clipping.process(*output_mesh);
    return PclMeshToglmMesh(output_mesh);
}