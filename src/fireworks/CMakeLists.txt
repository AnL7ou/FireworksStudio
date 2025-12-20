file(GLOB FIREWORKS_SRC *.cpp)

add_library(FireworksLib ${FIREWORKS_SRC} "Shape.h" "ShapeRegistry.cpp")

target_include_directories(FireworksLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FireworksLib PUBLIC VendorLibs)
