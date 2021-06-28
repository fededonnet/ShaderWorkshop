CMAKE_MINIMUM_REQUIRED(VERSION 3.19)

PROJECT(ShaderWorkshop CXX)

#Source Code:
SET(SRCS
	"${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/AssimpHelper.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/src/AssimpHelper.h"
	"${CMAKE_CURRENT_LIST_DIR}/src/camera.h"
)

SET(HDRS

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

add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/assimp-vc140-mtd.lib"
)

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "ShaderWorkshopMain")

add_executable(ShaderWorkshopMain "${SRCS}")
set_property(TARGET ShaderWorkshopMain PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_link_libraries(ShaderWorkshopMain PRIVATE glfw3 assimp opengl32 glad)

target_include_directories(ShaderWorkshopMain
	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/include"
)

file(COPY "${CMAKE_CURRENT_LIST_DIR}/lib/assimp-vc140-mtd.dll" DESTINATION "${CMAKE_BINARY_DIR}/Debug")