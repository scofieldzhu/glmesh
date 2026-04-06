set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL) 
message(STATUS "Found Qt version: ${Qt5_VERSION}")

set(CMAKE_AUTOUIC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)
set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)
# ENABLE CMP0020: Automatically link Qt executables to qtmain target on Windows.
cmake_policy(SET CMP0020 NEW)

find_package(PCL CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

# Target name
set(target glmesh.test)

set(SOURCE_FILES
    "main_window.h"
    "main_window.cpp"
    "main.cpp"
    "mainwindow.ui"
    "ply_reader.h"
    "ply_reader.cpp"
    "ply_reader.h"
    "ply_reader.cpp"
    "render_window.h"
    "render_window.cpp"
    "mesh_process.h"
    "mesh_process.cpp"
)

# Build executable
add_executable(${target}
    ${SOURCE_FILES}
)

target_include_directories(${target} PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} # for UIC generated headers
)

target_link_libraries(${target} PUBLIC
    ${PCL_LIBRARIES}
    Eigen3::Eigen
)

target_link_libraries(${target} PRIVATE
    ${DEFAULT_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    spdlog::spdlog
    glmesh.core
)

# # 
# # Compile definitions
# # 

# target_compile_definitions(${target}
#     PRIVATE
#     ${DEFAULT_COMPILE_DEFINITIONS}
#     GLM_ENABLE_EXPERIMENTAL
# )


# # 
# # Compile options
# # 

# target_compile_options(${target}
#     PRIVATE
#     ${DEFAULT_COMPILE_OPTIONS}
# )


# # 
# # Linker options
# # 

# target_link_libraries(${target}
#     PRIVATE
#     ${DEFAULT_LINKER_OPTIONS}
# )


# #
# # Target Health
# #

# perform_health_checks(
#     ${target}
#     ${sources}
# )


# # 
# # Deployment
# # 

# # Executable
# install(TARGETS ${target}
#     RUNTIME DESTINATION ${INSTALL_EXAMPLES} COMPONENT examples_qt
#     BUNDLE  DESTINATION ${INSTALL_EXAMPLES} COMPONENT examples_qt
# )
