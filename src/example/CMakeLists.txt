cmake_minimum_required(VERSION 3.20)
project(example)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} resources/resources.rc)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${reflection_core_headers})

target_link_libraries(${PROJECT_NAME}
    PRIVATE ${reflection_core_lib})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_options(${PROJECT_NAME} PRIVATE ${CUSTOM_COMPILE_OPTIONS})