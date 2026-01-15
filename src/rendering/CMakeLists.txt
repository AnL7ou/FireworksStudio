file(GLOB RENDERING_SRC *.cpp)

add_library(RenderingLib ${RENDERING_SRC} 
	"Shader.cpp"
	"ParticleRenderer.cpp"
	"Texture.cpp"
	"Camera.cpp"
	"TrailRenderer.cpp")

target_include_directories(RenderingLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RenderingLib PUBLIC VendorLibs)
