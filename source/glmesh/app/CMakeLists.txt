
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL) 
message(STATUS "Found Qt version: ${Qt5_VERSION}")

# 定义 UI 文件
set(QT_UI_FILES
    main_widget.ui
)

# 使用 qt5_wrap_ui 处理 UI 文件
qt5_wrap_ui(NP_UI_HEADERS ${QT_UI_FILES})

find_package(Eigen3 CONFIG REQUIRED)

# Target name
set(target "glmesh.app")

set(SOURCE_FILES
    "mesh_widget.h"
    "mesh_widget.cpp"
    "main.cpp"
    "main_widget.h"
    "main_widget.cpp"
)

# Build executable
add_executable(${target}
    ${SOURCE_FILES}
    ${QT_UI_FILES}
)

target_include_directories(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} # for UIC generated headers
)

target_link_libraries(${target} PUBLIC
    Eigen3::Eigen
)

target_link_libraries(${target} PRIVATE
    ${DEFAULT_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    spdlog::spdlog
    glmesh.kernel
    opengl32
)
