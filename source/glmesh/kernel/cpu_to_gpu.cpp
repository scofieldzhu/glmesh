/* 
*  glmesh is a mesh data render library base on QOpengl.
*  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
*  It reduces the amount of OpenGL code required for rendering and facilitates 
*  coherent OpenGL.
*  
*  File: cpu_to_gpu.cpp
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
#include "cpu_to_gpu.h"
#include <algorithm>
#include <pcl/PCLPointCloud2.h>
#include <pcl/PolygonMesh.h>
#include <pcl/conversions.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include "gl_triangle_mesh.h"
#include "glad.h"

GLMESH_NAMESPACE_BEGIN

// inline bool hasField(const pcl::PCLPointCloud2& cloud, std::string_view name)
// {
//     return std::any_of(
//         cloud.fields.begin(),
//         cloud.fields.end(),
//         [&](const pcl::PCLPointField&f){
//             return f.name == name;
//         }
//     );
// }

// template<class PointT, class Mapper>
// std::vector<CpuVertex> convertVertices(const pcl::PCLPointCloud2& src, Mapper&& mapper)
// {
//     pcl::PointCloud<PointT> cloud;
//     pcl::fromPCLPointCloud2(src, cloud);

//     std::vector<CpuVertex> out;
//     out.reserve(cloud.size());

//     for (const auto& p : cloud.points)
//     {
//         out.push_back(mapper(p));
//     }

//     return out;
// }

// inline CpuPolygonMesh fromPclPolygonMesh(const pcl::PolygonMesh& mesh)
// {
//     CpuPolygonMesh out;
//     const bool has_xyz = hasField(mesh.cloud, "x") && hasField(mesh.cloud, "y") && hasField(mesh.cloud, "z");
//     if (!has_xyz)
//     {
//         throw std::runtime_error("PCL mesh does not contain xyz fields");
//     }

//     const bool has_normal =
//         hasField(mesh.cloud, "normal_x") &&
//         hasField(mesh.cloud, "normal_y") &&
//         hasField(mesh.cloud, "normal_z");

//     const bool has_color =
//         hasField(mesh.cloud, "rgb") ||
//         hasField(mesh.cloud, "rgba");

//     // 1) xyz + normal + color
//     if (has_normal && has_color)
//     {
//         out.vertices = convertVertices<pcl::PointXYZRGBNormal>(
//             mesh.cloud,
//             [](const pcl::PointXYZRGBNormal& p)
//             {
//                 return CpuVertex{
//                     .position = {p.x, p.y, p.z},
//                     .normal   = {p.normal_x, p.normal_y, p.normal_z},
//                     .color    = {
//                         static_cast<float>(p.r) / 255.0f,
//                         static_cast<float>(p.g) / 255.0f,
//                         static_cast<float>(p.b) / 255.0f
//                     }
//                 };
//             });
//     }
//     // 2) xyz + normal
//     else if (has_normal)
//     {
//         out.vertices = convertVertices<pcl::PointNormal>(
//             mesh.cloud,
//             [](const pcl::PointNormal& p)
//             {
//                 return CpuVertex{
//                     .position = {p.x, p.y, p.z},
//                     .normal   = {p.normal_x, p.normal_y, p.normal_z},
//                     .color    = {1.0f, 1.0f, 1.0f}
//                 };
//             });
//     }
//     // 3) xyz + color
//     else if (has_color)
//     {
//         out.vertices = convertVertices<pcl::PointXYZRGB>(
//             mesh.cloud,
//             [](const pcl::PointXYZRGB& p)
//             {
//                 return CpuVertex{
//                     .position = {p.x, p.y, p.z},
//                     .normal   = {0.0f, 0.0f, 1.0f},
//                     .color    = {
//                         static_cast<float>(p.r) / 255.0f,
//                         static_cast<float>(p.g) / 255.0f,
//                         static_cast<float>(p.b) / 255.0f
//                     }
//                 };
//             });
//     }
//     // 4) only xyz
//     else
//     {
//         out.vertices = convertVertices<pcl::PointXYZ>(
//             mesh.cloud,
//             [](const pcl::PointXYZ& p)
//             {
//                 return CpuVertex{
//                     .position = {p.x, p.y, p.z},
//                     .normal   = {0.0f, 0.0f, 1.0f},
//                     .color    = {1.0f, 1.0f, 1.0f}
//                 };
//             });
//     }

//     out.faces.reserve(mesh.polygons.size());
//     for (const auto& poly : mesh.polygons)
//     {
//         PolygonFace face;
//         face.indices.reserve(poly.vertices.size());

//         for (const auto idx : poly.vertices)
//         {
//             face.indices.push_back(static_cast<std::uint32_t>(idx));
//         }

//         out.faces.push_back(std::move(face));
//     }

//     return out;
// }

// inline CpuPolygonMesh loadPlyAsCpuPolygonMesh(const std::string& path)
// {
//     pcl::PolygonMesh mesh;
//     if (pcl::io::loadPLYFile(path, mesh) < 0)
//     {
//         throw std::runtime_error("Failed to load PLY file: " + path);
//     }

//     return fromPclPolygonMesh(mesh);
// }

// CpuTriangleMesh triangulateFan(const CpuPolygonMesh& poly)
// {
//     CpuTriangleMesh tri;
//     tri.vertices = poly.vertices;

//     for (const auto& face : poly.faces)
//     {
//         const auto n = face.indices.size();
//         if (n < 3)
//         {
//             throw std::runtime_error("face has fewer than 3 vertices");
//         }

//         if (n == 3)
//         {
//             tri.indices.insert(tri.indices.end(), {
//                 face.indices[0], face.indices[1], face.indices[2]
//             });
//             continue;
//         }

//         // fan: (0, i, i+1)
//         for (std::size_t i = 1; i + 1 < n; ++i)
//         {
//             tri.indices.push_back(face.indices[0]);
//             tri.indices.push_back(face.indices[i]);
//             tri.indices.push_back(face.indices[i + 1]);
//         }
//     }

//     return tri;
// }

static const pcl::PCLPointField* findField(const pcl::PCLPointCloud2& cloud, const std::string& name)
{
    for (const auto& field : cloud.fields)
    {
        if (field.name == name)
        {
            return &field;
        }
    }
    return nullptr;
}

template <typename T>
static T readScalar(const std::uint8_t* pointBase, std::uint32_t offset)
{
    T value{};
    std::memcpy(&value, pointBase + offset, sizeof(T));
    return value;
}

static float readFloatLike(
    const std::uint8_t* pointBase,
    const pcl::PCLPointField* field,
    float defaultValue = 0.0f)
{
    if (field == nullptr)
    {
        return defaultValue;
    }

    switch (field->datatype)
    {
    case pcl::PCLPointField::INT8:
        return static_cast<float>(readScalar<std::int8_t>(pointBase, field->offset));
    case pcl::PCLPointField::UINT8:
        return static_cast<float>(readScalar<std::uint8_t>(pointBase, field->offset));
    case pcl::PCLPointField::INT16:
        return static_cast<float>(readScalar<std::int16_t>(pointBase, field->offset));
    case pcl::PCLPointField::UINT16:
        return static_cast<float>(readScalar<std::uint16_t>(pointBase, field->offset));
    case pcl::PCLPointField::INT32:
        return static_cast<float>(readScalar<std::int32_t>(pointBase, field->offset));
    case pcl::PCLPointField::UINT32:
        return static_cast<float>(readScalar<std::uint32_t>(pointBase, field->offset));
    case pcl::PCLPointField::FLOAT32:
        return readScalar<float>(pointBase, field->offset);
    case pcl::PCLPointField::FLOAT64:
        return static_cast<float>(readScalar<double>(pointBase, field->offset));
    default:
        return defaultValue;
    }
}

static glm::vec3 readColor(
    const std::uint8_t* pointBase,
    const pcl::PCLPointField* rgbField,
    const pcl::PCLPointField* rgbaField)
{
    const pcl::PCLPointField* field = rgbField ? rgbField : rgbaField;
    if (field == nullptr)
    {
        return {1.0f, 1.0f, 1.0f};
    }

    std::uint32_t packed = 0;

    switch (field->datatype)
    {
    case pcl::PCLPointField::FLOAT32:
    {
        float rgbAsFloat = readScalar<float>(pointBase, field->offset);
        std::memcpy(&packed, &rgbAsFloat, sizeof(float));
        break;
    }
    case pcl::PCLPointField::UINT32:
    case pcl::PCLPointField::INT32:
    {
        packed = readScalar<std::uint32_t>(pointBase, field->offset);
        break;
    }
    default:
        return {1.0f, 1.0f, 1.0f};
    }

    const float r = static_cast<float>((packed >> 16) & 0xFF) / 255.0f;
    const float g = static_cast<float>((packed >> 8)  & 0xFF) / 255.0f;
    const float b = static_cast<float>((packed >> 0)  & 0xFF) / 255.0f;

    return {r, g, b};
}

static CpuPolygonMesh loadPlyAsCpuPolygonMesh(const std::string& plyPath)
{
    pcl::PolygonMesh pclMesh;
    if (pcl::io::loadPLYFile(plyPath, pclMesh) < 0)
    {
        throw std::runtime_error("loadPLYFile() failed: " + plyPath);
    }

    const auto& cloud = pclMesh.cloud;
    if (cloud.point_step == 0)
    {
        throw std::runtime_error("invalid PLY vertex data: point_step == 0");
    }

    const auto* xField    = findField(cloud, "x");
    const auto* yField    = findField(cloud, "y");
    const auto* zField    = findField(cloud, "z");
    const auto* nxField   = findField(cloud, "normal_x");
    const auto* nyField   = findField(cloud, "normal_y");
    const auto* nzField   = findField(cloud, "normal_z");
    const auto* rgbField  = findField(cloud, "rgb");
    const auto* rgbaField = findField(cloud, "rgba");

    if (xField == nullptr || yField == nullptr || zField == nullptr)
    {
        throw std::runtime_error("PLY missing x/y/z fields");
    }

    CpuPolygonMesh mesh;

    const std::size_t pointCount = cloud.data.size() / cloud.point_step;
    mesh.vertices.resize(pointCount);

    for (std::size_t i = 0; i < pointCount; ++i)
    {
        const std::uint8_t* pointBase = cloud.data.data() + i * cloud.point_step;

        CpuVertex v;
        v.position = {
            readFloatLike(pointBase, xField),
            readFloatLike(pointBase, yField),
            readFloatLike(pointBase, zField)
        };

        v.normal = {
            readFloatLike(pointBase, nxField, 0.0f),
            readFloatLike(pointBase, nyField, 0.0f),
            readFloatLike(pointBase, nzField, 0.0f)
        };

        v.color = readColor(pointBase, rgbField, rgbaField);

        mesh.vertices[i] = v;
    }

    mesh.polygons.reserve(pclMesh.polygons.size());
    for (const auto& poly : pclMesh.polygons)
    {
        if (poly.vertices.size() < 3)
        {
            continue;
        }

        mesh.polygons.emplace_back(poly.vertices.begin(), poly.vertices.end());
    }

    return mesh;
}

static CpuTriangleMesh triangulateFan(const CpuPolygonMesh& polyMesh)
{
    CpuTriangleMesh triMesh;
    triMesh.vertices = polyMesh.vertices;

    for (const auto& poly : polyMesh.polygons)
    {
        if (poly.size() < 3)
        {
            continue;
        }

        for (std::size_t i = 1; i + 1 < poly.size(); ++i)
        {
            triMesh.indices.push_back(poly[0]);
            triMesh.indices.push_back(poly[i]);
            triMesh.indices.push_back(poly[i + 1]);
        }
    }

    return triMesh;
}

static std::vector<GpuVertex> buildGpuVertices(const std::vector<CpuVertex>& cpuVertices)
{
    std::vector<GpuVertex> gpuVertices;
    gpuVertices.reserve(cpuVertices.size());

    for (const auto& v : cpuVertices)
    {
        GpuVertex gv;
        gv.position = v.position;
        gv.normal   = v.normal;
        gv.color    = v.color;
        gpuVertices.push_back(gv);
    }

    return gpuVertices;
}

GLTriangleMesh LoadPlyRenderableMesh(const std::string& plyPath)
{
    const CpuPolygonMesh polygonMesh = loadPlyAsCpuPolygonMesh(plyPath);

    if (polygonMesh.polygons.empty())
    {
        throw std::runtime_error("this PLY has no face data; it is a point cloud, not a renderable triangle mesh");
    }

    const CpuTriangleMesh triangleMesh = triangulateFan(polygonMesh);
    const std::vector<GpuVertex> gpuVertices = buildGpuVertices(triangleMesh.vertices);

    GLTriangleMesh mesh;
    mesh.upload(gpuVertices, triangleMesh.indices, GL_STATIC_DRAW);
    return mesh;
}

GLMESH_NAMESPACE_END