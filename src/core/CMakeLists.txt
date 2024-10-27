cmake_minimum_required(VERSION 3.20)
project(reflection_core)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*)
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

set(${PROJECT_NAME}_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/reflection PARENT_SCOPE)
set(${PROJECT_NAME}_lib ${PROJECT_NAME} PARENT_SCOPE)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${ALL_DEPENDENCY_INCLUDES}
    PRIVATE ${EXT_INCLUDE_FILES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC ${DEPENDENCY_DIR}/webview2/lib/WebView2LoaderStatic.lib)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_options(${PROJECT_NAME} PRIVATE ${CUSTOM_COMPILE_OPTIONS})

add_custom_target(run_compiler
    COMMAND bun run ./core_compiler.ts $<$<CONFIG:Debug>:--debug> $<$<CONFIG:Release>:--release>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_dependencies(${PROJECT_NAME} run_compiler)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND lib "/OUT:$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.lib"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.lib"
        ${DEPENDENCY_DIR}/webview2/lib/WebView2LoaderStatic.lib
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Combining libraries..."
)