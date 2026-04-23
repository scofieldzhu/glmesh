
set(PKG_PROJ "glmesh.kernel")

set(EXPORT_HEAD_FILES
    glmesh_kernel_export.h
    glmesh_kernel_typedef.h
    gl/vertex_array.h
    gl/gpu_vertex.h
    gl/vertex_buffer.h
    gl/index_buffer.h
    gl/gl_triangle_mesh.h
    gl/gl_point_cloud.h
    gl/gl_api_init.h
    gl/shader_program.h
    gl/gl_debug.h
    core/cpu_vertex.h
    core/cpu_point_cloud.h
    core/cpu_polygon_mesh.h
    core/cpu_triangle_mesh.h
    core/cpu_to_gpu.h
    io/mesh_loader.h
    renderer.h
    material.h
)

set(SOURCE_FILES
    third_party/glad/src/glad.c    
    gl/gl_api_init.cpp
    gl/shader_program.cpp
    gl/gl_triangle_mesh.cpp
    gl/gl_point_cloud.cpp
    gl/vertex_array.cpp
    gl/vertex_buffer.cpp
    gl/gpu_vertex.cpp
    gl/index_buffer.cpp
    gl/gl_debug.cpp
    core/cpu_to_gpu.cpp
    io/mesh_loader.cpp
    material.cpp
    renderer.cpp
)

add_library(${PKG_PROJ}
    SHARED
    ${EXPORT_HEAD_FILES}
    ${SOURCE_FILES}
)

target_include_directories(${PKG_PROJ} PRIVATE
    third_party/glad/include
)

if(WIN32)
    target_compile_definitions(${PKG_PROJ} PRIVATE
        GLMESH_KERNEL_EXPORT
    )
endif()
