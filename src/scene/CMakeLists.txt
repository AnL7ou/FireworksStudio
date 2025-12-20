file(GLOB SCENE_SRC *.cpp)

add_library(SceneLib ${SCENE_SRC})

target_include_directories(SceneLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SceneLib PUBLIC VendorLibs)
