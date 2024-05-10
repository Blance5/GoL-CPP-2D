cmake_minimum_required(VERSION 3.10)
project(GoL2d)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Find GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Find GLFW
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS}
                    ${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/include/vendor)

set(SOURCES
    src/Main.cpp
    src/IndexBuffer.cpp
    src/Renderer.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/vendor/stb_image.cpp
)


# Add your executable and link libraries
add_executable(GoL2d ${SOURCES})
target_link_libraries(GoL2d ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)