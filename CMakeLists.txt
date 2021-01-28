CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

PROJECT(ShaderWorkshop CXX)

#Source Code:
SET(SRCS
	"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
)

add_library(glad STATIC "src/glad/glad.cpp")
target_include_directories(glad
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/include"
)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/glfw3.lib"
)

add_executable(ShaderWorkshopMain "${SRCS}")
set_property(TARGET ShaderWorkshopMain PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_link_libraries(ShaderWorkshopMain PRIVATE glfw3 opengl32 glad)

target_include_directories(ShaderWorkshopMain
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/include"
)