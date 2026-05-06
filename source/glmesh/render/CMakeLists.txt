
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
