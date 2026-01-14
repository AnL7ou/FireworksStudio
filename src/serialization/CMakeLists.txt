add_library(SerializationLib
    FireworkSerialization.cpp
    FireworkSerialization.h
)

target_include_directories(SerializationLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(SerializationLib PUBLIC
    VendorLibs
    FireworksLib
    SceneLib
)
