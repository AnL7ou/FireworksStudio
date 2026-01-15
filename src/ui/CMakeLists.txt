# Collecter tous les fichiers sources (.cpp)
file(GLOB_RECURSE UI_SRC
    # UIManager principal
    *.cpp
    
    # scene_editor
    panels/scene_editor/*.cpp
    
    # Template_editor
    panels/template_editor/*.cpp
    
    # Widgets
    widgets/*.cpp
)

add_library(UILib ${UI_SRC} "UIManager.h")
target_include_directories(UILib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UILib PUBLIC VendorLibs FireworksLib)