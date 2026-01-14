# CMakeLists.txt pour le module editor (authoring layer)

file(GLOB_RECURSE EDITOR_SOURCES
    *.cpp
)

set(EDITOR_HEADERS
    EditorContext.h
)

add_library(EditorLib ${EDITOR_SOURCES} ${EDITOR_HEADERS})

target_include_directories(EditorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(EditorLib PUBLIC VendorLibs FireworksLib)
