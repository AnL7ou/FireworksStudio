cmake_minimum_required(VERSION 3.10)

# Target INTERFACE qui porte toutes les libs/vendor
add_library(VendorLibs INTERFACE)

target_link_libraries(VendorLibs INTERFACE
    glfw
    glad
    imgui
    stbimage
    glm
)

target_include_directories(VendorLibs INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/glfw-3.3.8/include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_SOURCE_DIR}/vendor/stbimage/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui-1.91.1
    ${CMAKE_SOURCE_DIR}/vendor/glm-1.0.1
)

# Ensuite seulement :
add_subdirectory(core)
add_subdirectory(rendering)
add_subdirectory(fireworks)
add_subdirectory(scene)
add_subdirectory(ui)

add_executable(FireworksStudio main.cpp)
target_link_libraries(FireworksStudio PRIVATE
    CoreLib
    RenderingLib
    FireworksLib
    SceneLib
    UILib
)

add_dependencies(FireworksStudio copy_shaders)
