set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL) 
message(STATUS "Found Qt version: ${Qt5_VERSION}")

set(QT_UI_FILES
    main_widget.ui
)
qt5_wrap_ui(NP_UI_HEADERS ${QT_UI_FILES})

set(target "MeshStudio")

set(SOURCE_FILES
    "app_log.h"
    "app_log.cpp"
    "mesh_widget.h"
    "mesh_widget.cpp"
    "main.cpp"
    "main_widget.h"
    "main_widget.cpp"
    "renderable_object.h"
    "resource_util.h"
    "resource_util.cpp"
    "shader_program_manager.h"
    "shader_program_manager.cpp"
    ${CMAKE_SOURCE_DIR}/res/resource.qrc
)

add_executable(${target}
    ${SOURCE_FILES}
    ${QT_UI_FILES}
    shader_program_manager.h
)

target_include_directories(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} # for UIC generated headers
)

target_link_libraries(${target} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    glmesh.kernel
    opengl32
)
