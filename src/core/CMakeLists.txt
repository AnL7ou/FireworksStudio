file(GLOB CORE_SRC *.cpp)

add_library(CoreLib ${CORE_SRC}
	"Application.h"
	"Application.cpp"
	"InputRouter.h"
	"InputRouter.cpp"
	"OrbitalCameraController.h"
	"OrbitalCameraController.cpp"
	"Window.h"
	"Window.cpp"
)

target_include_directories(CoreLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CoreLib PUBLIC VendorLibs)
