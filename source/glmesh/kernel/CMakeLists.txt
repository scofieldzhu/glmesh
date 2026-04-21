
set(PKG_PROJ "glmesh.kernel")

set(EXPORT_HEAD_FILES
    "glm_kernel_export.h"
    "glm_kernel_basetype.h"
    "vertex_array.h"
    "gpu_vertex.h"
    "cpu_vertex.h"
    "cpu_point_cloud.h"
    "cpu_polygon_mesh.h"
    "cpu_triangle_mesh.h"
    "vertex_buffer.h"
    "index_buffer.h"
    "gl_triangle_mesh.h"
    "gl_point_cloud.h"
    "cpu_to_gpu.h"    
    "shader_program.h"
    "renderer.h"
    "material.h"
    "gl_init.h"
)

set(SOURCE_FILES
    "gl_init.cpp"
    "khrplatform.h"
    "cpu_to_gpu.cpp"
    "material.cpp"
    "renderer.cpp"
    "shader_program.cpp"
    "glad.h"
    "glad.c"
    "gl_triangle_mesh.cpp"
    "gl_point_cloud.cpp"
    "vertex_array.cpp"
    "vertex_buffer.cpp"
    "gpu_vertex.cpp"
    "index_buffer.cpp"
)

add_library(${PKG_PROJ}
    SHARED
    ${EXPORT_HEAD_FILES}
    ${SOURCE_FILES}
)

if(WIN32)
    target_compile_definitions(${PKG_PROJ} PRIVATE
        GLMESH_KERNEL_EXPORT
    )
endif()
