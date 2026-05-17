
set(PKG_PROJ "glmesh.render")

set(EXPORT_HEAD_FILES
    glmesh_render_export.h
    glmesh_render_typedef.h
    camera.h
)

set(SOURCE_FILES
    camera.cpp
)

add_library(${PKG_PROJ} SHARED
    ${EXPORT_HEAD_FILES}
    ${SOURCE_FILES}    
)

if(MSVC)
    target_compile_definitions(${PKG_PROJ} PRIVATE
        GLMESH_RENDER_EXPORT
    )
endif()

# --- install rules ---
install(TARGETS ${PKG_PROJ}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}")
foreach(hdr ${EXPORT_HEAD_FILES})
    get_filename_component(hdr_dir "${hdr}" DIRECTORY)
    if(hdr_dir)
        install(FILES "${hdr}" DESTINATION "include/glmesh/render/${hdr_dir}")
    else()
        install(FILES "${hdr}" DESTINATION "include/glmesh/render")
    endif()
endforeach()
