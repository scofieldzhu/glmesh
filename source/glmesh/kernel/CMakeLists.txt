
set(PKG_PROJ "glmesh.kernel")

set(EXPORT_HEAD_FILES
    glmesh_kernel_export.h
    glmesh_kernel_typedef.h
    gl/gl_drawable.h
    gl/vertex_array.h
    gl/gpu_vertex.h
    gl/vertex_buffer.h
    gl/index_buffer.h
    gl/gl_triangle_mesh.h
    gl/gpu_triangle_mesh.h
    gl/gl_point_cloud.h
    gl/gpu_rectangle.h
    gl/gl_rectangle.h
    gl/gl_api_init.h
    gl/shader_program.h
    gl/gl_debug.h
    core/cpu_vertex.h
    core/cpu_point_cloud.h
    core/cpu_polygon_mesh.h
    core/cpu_triangle_mesh.h    
    core/cpu_rectangle.h
    core/mesh_bounds.h
    core/geometry_utils.h
    io/mesh_loader.h
    cpu_to_gpu.h
    material.h
    glmesh_log.h
)

set(SOURCE_FILES
    third_party/glad/src/glad.c    
    gl/gl_api_init.cpp
    gl/shader_program.cpp
    gl/gl_triangle_mesh.cpp
    gl/gl_point_cloud.cpp
    gl/gl_rectangle.cpp
    gl/vertex_array.cpp
    gl/vertex_buffer.cpp
    gl/gpu_vertex.cpp
    gl/index_buffer.cpp
    gl/gl_debug.cpp    
    core/cpu_triangle_mesh.cpp
    core/geometry_utils.cpp
    io/mesh_loader.cpp
    cpu_to_gpu.cpp
    material.cpp
    glmesh_log.cpp
)

add_library(${PKG_PROJ}
    SHARED
    ${EXPORT_HEAD_FILES}
    ${SOURCE_FILES}
    gl/gpu_bkg_vertex.h
    core/cpu_bkg_vertex.h
    core/cpu_bkg.h
    gl/gpu_bkg.h
    gl/gpu_bkg_vertex.cpp
    gl/gl_bkg.h
    gl/gl_bkg.cpp
)

target_include_directories(${PKG_PROJ} PRIVATE
    third_party/glad/include
)

if(MSVC)
    target_compile_definitions(${PKG_PROJ} PRIVATE
        GLMESH_KERNEL_EXPORT
    )
endif()
