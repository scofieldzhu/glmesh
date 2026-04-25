/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: mesh_loader.cpp
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
#include "mesh_loader.h"
#include <algorithm>
#include <pcl/PCLPointCloud2.h>
#include <pcl/PolygonMesh.h>
#include <pcl/conversions.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include "glad/glad.h"
#include "glmesh/kernel/gl/gl_triangle_mesh.h"
#include "glmesh/kernel/core/cpu_polygon_mesh.h"
#include "glmesh/kernel/core/cpu_triangle_mesh.h"
#include "glmesh/kernel/core/cpu_mesh_conv.h"
#include "glmesh/kernel/core/cpu_to_gpu.h"
#include "glmesh/kernel/glmesh_log.h"

GLMESH_NAMESPACE_BEGIN

namespace{
    const pcl::PCLPointField* FindPointField(const pcl::PCLPointCloud2& cloud, const std::string& name)
    {
        for(const auto& field : cloud.fields){
            if(field.name == name){
                return &field;
            }
        }
        return nullptr;
    }

    template <typename T>
    T ReadScalarValue(const std::uint8_t* data_base, std::uint32_t offset)
    {
        T value;
        std::memcpy(&value, data_base + offset, sizeof(T));
        return value;
    }

    float ReadFloatFieldValue(const std::uint8_t* point_base, const pcl::PCLPointField* field)
    {
        assert(field);
        switch(field->datatype){
            case pcl::PCLPointField::INT8:
                return static_cast<float>(ReadScalarValue<std::int8_t>(point_base, field->offset));
            case pcl::PCLPointField::UINT8:
                return static_cast<float>(ReadScalarValue<std::uint8_t>(point_base, field->offset));
            case pcl::PCLPointField::INT16:
                return static_cast<float>(ReadScalarValue<std::int16_t>(point_base, field->offset));
            case pcl::PCLPointField::UINT16:
                return static_cast<float>(ReadScalarValue<std::uint16_t>(point_base, field->offset));
            case pcl::PCLPointField::INT32:
                return static_cast<float>(ReadScalarValue<std::int32_t>(point_base, field->offset));
            case pcl::PCLPointField::UINT32:
                return static_cast<float>(ReadScalarValue<std::uint32_t>(point_base, field->offset));
            case pcl::PCLPointField::FLOAT32:
                return ReadScalarValue<float>(point_base, field->offset);
            case pcl::PCLPointField::FLOAT64:
                return static_cast<float>(ReadScalarValue<double>(point_base, field->offset));
            default:
                break;
        }
        return 0.0f;
    }

    glm::vec3 ReadColorFieldValue(const std::uint8_t* point_base, const pcl::PCLPointField* rgb_field, const pcl::PCLPointField* rgba_field)
    {
        auto clr_field = rgb_field ? rgb_field : rgba_field;
        if(clr_field == nullptr){
            return {1.0f, 1.0f, 1.0f};
        }
        uint32_t packed = 0;
        switch(clr_field->datatype){
            case pcl::PCLPointField::FLOAT32:
            {
                float float_rgb = ReadScalarValue<float>(point_base, clr_field->offset);
                std::memcpy(&packed, &float_rgb, sizeof(float));
                break;
            }
            case pcl::PCLPointField::UINT32:
            case pcl::PCLPointField::INT32:
            {
                packed = ReadScalarValue<std::uint32_t>(point_base, clr_field->offset);
                break;
            }
            default:
                return {1.0f, 1.0f, 1.0f};
        }
        float r = static_cast<float>((packed >> 16) & 0xFF) / 255.0f;
        float g = static_cast<float>((packed >> 8)  & 0xFF) / 255.0f;
        float b = static_cast<float>((packed >> 0)  & 0xFF) / 255.0f;
        return {r, g, b};
    }
}

bool LoadPlyAsCpuPolygonMesh(const std::string& ply_filepath, CpuPolygonMesh& out_mesh, MeshLoadError* out_err)
{
    pcl::PolygonMesh pcl_mesh;
    if(pcl::io::loadPLYFile(ply_filepath, pcl_mesh) < 0){
        if(out_err){
            *out_err = MeshLoadError::FileNotFound;
        }
        return false;
    }
    const auto& cloud = pcl_mesh.cloud;
    if(cloud.point_step == 0){
        if(out_err){
            *out_err = MeshLoadError::ParseFailed;
        }
        return false;
    }
    auto x_field = FindPointField(cloud, "x");
    auto y_field = FindPointField(cloud, "y");
    auto z_field = FindPointField(cloud, "z");
    if(x_field == nullptr || y_field == nullptr || z_field == nullptr){
        if(out_err){
            *out_err = MeshLoadError::ParseFailed;
        }
        return false;
    }
    auto nx_field = FindPointField(cloud, "normal_x");
    auto ny_field = FindPointField(cloud, "normal_y");
    auto nz_field = FindPointField(cloud, "normal_z");
    auto read_normal_func = [nx_field, ny_field, nz_field](const uint8* point_base){
        if(nx_field == nullptr || ny_field == nullptr || nz_field == nullptr){
            return glm::vec3(0.0, 0.0, 0.0);
        }
        glm::vec3 n;
        n[0] = ReadFloatFieldValue(point_base, nx_field);
        n[0] = ReadFloatFieldValue(point_base, ny_field);
        n[0] = ReadFloatFieldValue(point_base, nz_field);
        return n;
    };
    auto rgb_field = FindPointField(cloud, "rgb");
    auto rgba_field = FindPointField(cloud, "rgba");
    CpuPolygonMesh mesh;
    const auto point_cnt = cloud.data.size() / cloud.point_step;
    mesh.vertices.resize(point_cnt);
    for(auto i = 0; i < point_cnt; ++i){
        auto point_base = cloud.data.data() + i * cloud.point_step;
        CpuVertex v;
        v.position = {
            ReadFloatFieldValue(point_base, x_field),
            ReadFloatFieldValue(point_base, y_field),
            ReadFloatFieldValue(point_base, z_field)
        };
        v.normal = read_normal_func(point_base);
        v.color = ReadColorFieldValue(point_base, rgb_field, rgba_field);
        mesh.vertices[i] = v;
    }
    mesh.polygons.reserve(pcl_mesh.polygons.size());
    for(const auto& poly : pcl_mesh.polygons){
        if(poly.vertices.size() < 3){
            continue;
        }
        mesh.polygons.emplace_back(poly.vertices.begin(), poly.vertices.end());
    }
    out_mesh = std::move(mesh);
    return true;
}

bool LoadPlyRenderableMesh(const std::string& ply_filepath, GLTriangleMesh& out_mesh, MeshLoadError* out_err)
{
    CpuPolygonMesh polygon_mesh;
    if(!LoadPlyAsCpuPolygonMesh(ply_filepath, polygon_mesh, out_err)){
        return false;
    }
    if(polygon_mesh.polygons.empty()){
        if(out_err){
            *out_err = MeshLoadError::InvalidTopology;
        }
        GLMESH_LOG_ERROR("This PLY has no face data; Although it is a point cloud, not a renderable triangle mesh");
        return false;
    }
    CpuTriangleMesh triangle_mesh;
    PolygonToTriangleMesh(polygon_mesh, triangle_mesh);
    auto gpu_vertices = ToGpuVertices(triangle_mesh.vertices);
    GLTriangleMesh mesh;
    mesh.upload(gpu_vertices, triangle_mesh.indices, GL_STATIC_DRAW);
    out_mesh = std::move(mesh);
    return true;
}

GLMESH_NAMESPACE_END