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
include_directories(${GLFW_INCLUDE_DIRS})

# Add your executable and link libraries
add_executable(GoL2d test.cpp IndexBuffer.cpp Renderer.cpp VertexArray.cpp VertexBuffer.cpp)
target_link_libraries(GoL2d ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)