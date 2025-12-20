file(GLOB UI_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/panels/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp"
)
add_library(UILib ${UI_SRC} "UIManager.h")

target_include_directories(UILib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UILib PUBLIC VendorLibs)

