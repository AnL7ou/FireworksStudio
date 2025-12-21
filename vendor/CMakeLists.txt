cmake_minimum_required(VERSION 3.5)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CMAKE_BUILD_TYPE (optionnel)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose build type: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenGL
find_package(OpenGL REQUIRED)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OpenGL_GL_PREFERENCE "LEGACY")
endif()

# stb_image
add_subdirectory(stbimage)
include_directories(stbimage/include)

# GLAD
add_subdirectory(glad)
include_directories(glad/include)

# GLFW
add_subdirectory(glfw-3.3.8)
include_directories(glfw-3.3.8/include)

# GLM
add_subdirectory(glm-1.0.1/glm)
include_directories(glm-1.0.1)

# ImGui
add_subdirectory(imgui-1.91.1)
include_directories(imgui-1.91.1)
