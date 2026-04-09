
set(PROJECT_NAME "glmesh.core")

project(${PROJECT_NAME})

find_package(glm REQUIRED COMPONENTS glm::glm)

set(EXPORT_HEAD_FILES
    "glm_nsp.h"
    "glm_export.h"
    "glm_base_type.h"
    "buffer.h"
    "memory_block.h"
    "instantiator.h"
    "mesh_poly_data.h"
    "mesh_actor.h"
    "actor.h"
    "camera.h"
    "mesh_renderer.h"
    "shader_program.h"
    "trackball.h"
    "sphere_actor.h"
    "vertex_array.h"
    "vertex_array_attrib.h"
    "win_event.h"
    "cube.h"
    "win_event_handler.h"
    "win_event_handler_publisher.h"
)

set(SOURCE_FILES
    "glad/glad.h"
    "KHR/khrplatform.h"
    "glad.c"
    "buffer.cpp"
    "cube.cpp"
    "actor.cpp"
    "memory_block.cpp"
    "mesh_renderer.cpp"
    "mesh_actor.cpp"
    "mesh_poly_data.cpp"
    "misc.h"
    "misc.cpp"
    "camera.cpp"
    "bkg_actor.cpp"
    "bkg_actor.h"
    "sphere_actor.cpp"
    "shader_program.cpp"
    "trackball.cpp"
    "vertex_array_attrib.cpp"
    "vertex_array.cpp"
    "win_event_handler_publisher.cpp"
    "shader_source.h"
    "shader_source.h.in"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/shader_source.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader_source.h"
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
    ${INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    spdlog::spdlog
    glm::glm
)