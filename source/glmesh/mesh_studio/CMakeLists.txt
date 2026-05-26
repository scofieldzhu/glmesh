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
    "common.h"
    "material.h"
    "material.cpp"
    "arcball_rotator.h"
    "arcball_rotator.cpp"
    "mouse_interaction.h"
    "orbit_interaction.h"
    "orbit_interaction.cpp"
    "color_button.h"
    "color_button.cpp"
    ${CMAKE_SOURCE_DIR}/res/resource.qrc
)

add_executable(${target}
    ${SOURCE_FILES}
    ${QT_UI_FILES}    
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
    glmesh.render
    opengl32
)

set_target_properties(${target} PROPERTIES
    DEBUG_POSTFIX "-d"
)

# --- install rules ---
install(TARGETS ${target}
    RUNTIME DESTINATION bin
)

# --- Install runtime dependencies on Windows ---
if(WIN32)
    get_target_property(qt_core_location Qt5::Core LOCATION)
    get_filename_component(qt_bin_dir "${qt_core_location}" DIRECTORY)

    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt
        HINTS "${qt_bin_dir}"
    )

    if(NOT WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "windeployqt not found")
    endif()

    # Copy all DLLs from build output to install bin (vcpkg deps: PCL, spdlog, etc.)
    install(CODE "
        file(GLOB build_dlls \"${CMAKE_BINARY_DIR}/bin/*.dll\")
        foreach(dll_path \${build_dlls})
            file(COPY \"\${dll_path}\" DESTINATION \"\${CMAKE_INSTALL_PREFIX}/bin\")
            get_filename_component(dll_name \"\${dll_path}\" NAME)
            message(STATUS \"Copied: \${dll_name}\")
        endforeach()
    ")

    # Run windeployqt on the installed binary to deploy Qt DLLs, platform plugin, and MSVC runtime
    install(CODE "
        file(GLOB _installed_exes \"\${CMAKE_INSTALL_PREFIX}/bin/${target}*.exe\")
        if(_installed_exes)
            list(GET _installed_exes 0 _installed_exe)
            message(STATUS \"Running windeployqt on \${_installed_exe}...\")
            execute_process(
                COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                    --no-translations
                    \"\${_installed_exe}\"
                RESULT_VARIABLE _wdqt_result
                OUTPUT_VARIABLE _wdqt_out
                ERROR_VARIABLE _wdqt_err
            )
            if(NOT _wdqt_result EQUAL 0)
                message(WARNING \"windeployqt failed (\${_wdqt_result}): \${_wdqt_err}\")
            else()
                message(STATUS \"windeployqt completed.\")
            endif()
        else()
            message(WARNING \"No ${target} binary found in \${CMAKE_INSTALL_PREFIX}/bin/ — windeployqt skipped.\")
        endif()
    ")
endif()
