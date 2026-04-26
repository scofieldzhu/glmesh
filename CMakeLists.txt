cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project("glmesh" VERSION 1.0)

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)  

message(STATUS "IS_MULTI_CONFIG = ${IS_MULTI_CONFIG}")

if(NOT IS_MULTI_CONFIG)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type")  
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)  
  endif()
  message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}!")
else()
  set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "Available build configurations" FORCE)
endif()

if(MSVC)
    if(MSVC_VERSION GREATER_EQUAL 1920)             
        set(CMAKE_CXX_STANDARD 20) 
        message(STATUS "Configuring for Visual Studio 2019 or newer, set CMAKE_CXX_STANDARD to 20.")
    else()
        message(FATAL_ERROR "MSVC version must be at least 19.0!")
    endif()
else()
    message(STATUS "Configuring for Non-VS tool, set CMAKE_CXX_STANDARD to 20.")
    set(CMAKE_CXX_STANDARD 20) #changed to 20 in future
endif()
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(spdlog REQUIRED)
message(STATUS "Find spdlog library! ${SPDLOG_VERSION}")
find_package(glm REQUIRED)
message(STATUS "Find glm library! ${GLM_VERSION}")
find_package(PCL REQUIRED)
message(STATUS "Find pcl library! ${PCL_VERSION}")

add_definitions(
  -DGLM_ENABLE_EXPERIMENTAL
)

add_compile_definitions(
  ${PCL_DEFINITIONS}  
)

add_compile_options(
  /wd4996 
  /wd4275 
  /wd4251 
  /wd4312
)

include_directories(
  ${CMAKE_SOURCE_DIR}/source
  ${PCL_INCLUDE_DIRS}
)

link_libraries(
  ${PCL_LIBRARIES}
  spdlog::spdlog
  glm::glm
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

#add_subdirectory(source/${PROJECT_NAME}/core)
add_subdirectory(source/${PROJECT_NAME}/kernel)
#add_subdirectory(source/${PROJECT_NAME}/test)
add_subdirectory(source/${PROJECT_NAME}/app)