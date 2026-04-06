
set(PROJECT_NAME "glmesh.core")

project(${PROJECT_NAME})

find_package(glm REQUIRED COMPONENTS glm::glm)

set(EXPORT_HEAD_FILES
    "glm_nsp.h"
    "glm_export.h"
    "glm_base_type.h"
    "glm_buffer.h"
    "glm_memory_block.h"
    "glm_instantiator.h"
    "glm_mesh.h"
    "glm_mesh_actor.h"
    "glm_actor.h"
    "glm_camera.h"
    "glm_mesh_renderer.h"
    "glm_shader_program.h"
    "glm_trackball.h"
    "glm_sphere_actor.h"
    "glm_vertex_array.h"
    "glm_vertex_array_attrib.h"
    "glm_win_event.h"
    "glm_cube.h"
    "glm_win_event_handler.h"
    "glm_win_event_handler_publisher.h"
)

set(SOURCE_FILES
    "glad/glad.h"
    "KHR/khrplatform.h"
    "glad.c"
    "glm_buffer.cpp"
    "glm_cube.cpp"
    "glm_mesh_actor.cpp"
    "glm_actor.cpp"
    "glm_memory_block.cpp"
    "glm_mesh_renderer.cpp"
    "glm_mesh.cpp"
    "glm_misc.h"
    "glm_misc.cpp"
    "glm_camera.cpp"
    "glm_bkg_actor.cpp"
    "glm_bkg_actor.h"
    "glm_sphere_actor.cpp"
    "glm_shader_program.cpp"
    "glm_trackball.cpp"
    "glm_vertex_array_attrib.cpp"
    "glm_vertex_array.cpp"
    "glm_win_event_handler_publisher.cpp"
    "glm_shader_source.h"
    "glm_shader_source.h.in"
    "shader.vert"
    "shader.frag"
    "bkg_shader.vert"
    "bkg_shader.frag"
    "cube.vert"
    "cube.frag"
)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/shader.vert" VERTEX_SHADER_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/bkg_shader.vert" BKG_VERTEX_SHADER_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cube.vert" CUBE_VERTEX_SHADER_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/shader.frag" FRAGMENT_SHADER_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/bkg_shader.frag" BKG_FRAGMENT_SHADER_SOURCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/cube.frag" CUBE_FRAGMENT_SHADER_SOURCE)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/glm_shader_source.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/glm_shader_source.h"
)

add_library(${PROJECT_NAME}
    SHARED
    ${EXPORT_HEAD_FILES}
    ${SOURCE_FILES}
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLMESH_EXPORT
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    spdlog::spdlog
    glm::glm
)