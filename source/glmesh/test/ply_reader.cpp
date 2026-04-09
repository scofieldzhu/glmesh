/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: ply_reader.cpp 
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

#include "ply_reader.h"
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h> 
#include <pcl/point_cloud.h> 
#include <pcl/PolygonMesh.h>
#include <pcl/surface/ear_clipping.h>
#include <spdlog/spdlog.h>
using namespace pcl;
using namespace glmesh;

bool ply_reader::LoadFile(const QString& filename, glmesh::MeshPolyData& result_mesh, bool need_triangulate)
{
    pcl::PolygonMesh mesh;
    if (pcl::io::loadPLYFile(filename.toLocal8Bit().toStdString().c_str(), mesh) == -1) {
        spdlog::error("Read mesh data from ply file:'{}' failed!", filename.toLocal8Bit().toStdString());
        return false;
    }
    if(mesh.cloud.fields.empty()){
        spdlog::error("Error: No vertex data found in the PLY file:{}.", filename.toLocal8Bit().toStdString());
        return false;
    }
    result_mesh.vertices.clear();
    for(size_t i = 0; i < mesh.cloud.data.size(); i += mesh.cloud.point_step){
        float x = *reinterpret_cast<float*>(&mesh.cloud.data[i + mesh.cloud.fields[0].offset]);
        float y = *reinterpret_cast<float*>(&mesh.cloud.data[i + mesh.cloud.fields[1].offset]);
        float z = *reinterpret_cast<float*>(&mesh.cloud.data[i + mesh.cloud.fields[2].offset]);
        result_mesh.vertices.push_back({x, y, z}); 
    }
    result_mesh.colors.clear();
    if(mesh.cloud.fields.size() > 3){
        pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);
        pcl::fromPCLPointCloud2(mesh.cloud, *cloud);
        for(const auto& point : *cloud){
            uint8_t r = point.r;
            uint8_t g = point.g;
            uint8_t b = point.b;
            uint8_t a = point.a;  
            glm::vec4 clr = {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
            result_mesh.colors.push_back(std::move(clr));
        }
    }        
    if(mesh.polygons.empty()){
        spdlog::warn("No facet data found in ply file:{}!", filename.toLocal8Bit().toStdString());
        return true;
    }
    if(!need_triangulate){   
        result_mesh.poly_facets.clear();
        for(const auto& polygon : mesh.polygons){
            glmPolyFacet pf;
            for(auto idx : polygon.vertices)
                pf.push_back(idx);
            result_mesh.poly_facets.push_back(std::move(pf));
        }
    }else{
        // triangulation!
        result_mesh.triangle_facets.clear();
        pcl::EarClipping ear_clipping;
        ear_clipping.setInputMesh(std::make_shared<pcl::PolygonMesh>(mesh));
        pcl::PolygonMesh output_mesh;
        ear_clipping.process(output_mesh);
        for(const auto& polygon : mesh.polygons){
            assert(polygon.vertices.size() == 3);
            result_mesh.triangle_facets.push_back(glmTriangleFacet(polygon.vertices[0], polygon.vertices[1], polygon.vertices[2]));
        }
    }
    spdlog::info("Vertices count:{} Color count:{} Triangle facet count:{} polygon facet count:{}!", result_mesh.vertices.size(), result_mesh.colors.size(), result_mesh.triangle_facets.size(), result_mesh.poly_facets.size());
    SPDLOG_INFO("Read cloud file:{} successfully!", filename.toLocal8Bit().toStdString());        
    return true;
}