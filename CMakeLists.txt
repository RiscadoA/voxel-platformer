cmake_minimum_required(VERSION 3.7)
project(voxel-platformer VERSION 0.1.0 LANGUAGES CXX)

set (SOURCE_FILES
	"src/vpg/entry.cpp"
	"src/vpg/config.hpp"
	"src/vpg/config.cpp"
	
	
)
	
add_executable(vpg ${SOURCE_FILES})
set_target_properties(vpg PROPERTIES
	CXX_STANDARD 17
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(vpg
	PRIVATE
		VPG_VERSION="${PROJECT_VERSION}"
		VPG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
		VPG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
		VPG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
		GLEW_STATIC
)

target_include_directories(vpg
	PRIVATE
		"src/"
)

# Dependencies
find_package(GLEW REQUIRED)
target_link_libraries(vpg PRIVATE GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(vpg PRIVATE OpenGL::GL)

find_package(glm REQUIRED)
target_link_libraries(vpg PRIVATE glm)

find_package(glfw3 REQUIRED)
target_link_libraries(vpg PRIVATE glfw)

find_package(freetype CONFIG REQUIRED)
target_link_libraries(vpg PRIVATE freetype)
