file(GLOB CORE_SRC *.cpp)

add_library(CoreLib ${CORE_SRC})

target_include_directories(CoreLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CoreLib PUBLIC VendorLibs)
