cmake_minimum_required(VERSION 3.12)
project(landscape)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(GLFW_DIR        ${PROJECT_SOURCE_DIR}/lib/glfw)
set(GLFW_BINARY_DIR ${GLFW_DIR}/bin)
set(GLM_DIR         ${PROJECT_SOURCE_DIR}/lib/glm)
set(GLM_BINARY_DIR  ${GLM_DIR}/bin)

ExternalProject_Add(
        glfw
        PREFIX         ${GLFW_DIR}
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3
        INSTALL_DIR    ${GLFW_BINARY_DIR}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${GLFW_BINARY_DIR}
        -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF)
ExternalProject_Add(
        glm
        PREFIX         ${GLM_DIR}
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        0.9.9.3
        INSTALL_DIR    ${GLM_BINARY_DIR}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${GLM_BINARY_DIR}
        -DGLM_TEST_ENABLE=OFF)

include_directories(
        ${GLFW_BINARY_DIR}/include
        ${GLM_BINARY_DIR}/include
        lib/glad/include
        include
        lib)
link_directories(${GLM_BINARY_DIR}/lib)
link_libraries(${GLFW_BINARY_DIR}/lib/libglfw.3.dylib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/)

set(SOURCES_GLAD lib/glad/src/glad.c)
set(SOURCES
        ${SOURCES_GLAD}
        src/main.cpp
        src/file_loader.cpp
        src/graphics/shader.cpp
        src/graphics/image.cpp
        src/graphics/camera.cpp
        src/graphics/texture.cpp
        src/graphics/framebuffer.cpp
        src/graphics/mesh.cpp
        src/graphics/primitives/cube.cpp
        src/graphics/primitives/plane.cpp
        src/graphics/model.cpp
        src/graphics/noise.cpp)

add_executable(landscape ${SOURCES})
add_dependencies(landscape glfw glm)

add_custom_command(
        TARGET landscape 
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${GLFW_BINARY_DIR}/lib/libglfw.3.dylib
        ${CMAKE_BINARY_DIR}/output/lib/libglfw.3.dylib)
add_custom_command(
        TARGET landscape
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/src/glsl
        ${CMAKE_BINARY_DIR}/output/glsl)
add_custom_command(
        TARGET landscape
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/output/assets)
