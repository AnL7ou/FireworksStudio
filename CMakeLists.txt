cmake_minimum_required(VERSION 3.10)
project(FireworksStudio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Vendor (GLFW, GLAD, ImGui, etc.)
add_subdirectory(vendor)

# Copie des shaders dans le dossier de build
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "${CMAKE_BINARY_DIR}/shaders"
    COMMENT "Copying shaders to build directory"
)

if(EXISTS "${CMAKE_SOURCE_DIR}/assets/shapes")
  file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
  message(STATUS "Copied assets -> ${CMAKE_BINARY_DIR}/assets")
endif()

# Source code
add_subdirectory(src)