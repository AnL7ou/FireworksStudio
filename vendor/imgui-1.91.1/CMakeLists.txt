project(imgui)
cmake_minimum_required(VERSION 3.5)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/")
set(INC_DIR "${PROJECT_SOURCE_DIR}/")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include BEFORE)

set(SRC
  ${SRC_DIR}/imgui_demo.cpp
  ${SRC_DIR}/imgui_draw.cpp
  ${SRC_DIR}/backends/imgui_impl_glfw.cpp
  ${SRC_DIR}/backends/imgui_impl_opengl3.cpp
  ${SRC_DIR}/misc/cpp/imgui_stdlib.cpp
  ${SRC_DIR}/imgui_tables.cpp
  ${SRC_DIR}/imgui_widgets.cpp
  ${SRC_DIR}/imgui.cpp
  # ${SRC_DIR}/misc/freetype/imgui_freetype.cpp
)

set(HEADER
  ${INC_DIR}/imconfig.h
  ${INC_DIR}/imgui.h
  ${INC_DIR}/imgui_internal.h
  ${INC_DIR}/imstb_rectpack.h
  ${INC_DIR}/imstb_textedit.h
  ${INC_DIR}/imstb_truetype.h
  ${INC_DIR}/backends/imgui_impl_glfw.h
  ${INC_DIR}/backends/imgui_impl_opengl3_loader.h
  ${INC_DIR}/backends/imgui_impl_opengl3.h
  ${INC_DIR}/misc/cpp/imgui_stdlib.h
  # ${INC_DIR}/misc/freetype/imgui_freetype.h
)

set(RESOURCE_FILES
)

add_library(${PROJECT_NAME} ${SRC} ${HEADER} ${RESOURCE_FILES})

# Compile definitions pour ImGui
target_compile_definitions(${PROJECT_NAME} PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLFW_INCLUDE_NONE
)

# Include directories
target_include_directories(${PROJECT_NAME}
  PUBLIC 
    ${INC_DIR}
    ${INC_DIR}/backends
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    glad
    glfw
)

install(
	TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(MSVC)
    target_compile_options(imgui PRIVATE /wd5045)
endif()
