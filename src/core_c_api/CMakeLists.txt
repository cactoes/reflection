cmake_minimum_required(VERSION 3.20)
project(reflection_core_c_api)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set(${PROJECT_NAME}_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/reflection PARENT_SCOPE)
set(${PROJECT_NAME}_lib ${PROJECT_NAME} PARENT_SCOPE)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${reflection_core_headers}
    PRIVATE include)

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${reflection_core_lib})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_options(${PROJECT_NAME} PRIVATE ${CUSTOM_COMPILE_OPTIONS})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE REFLECTION_EXPORT)