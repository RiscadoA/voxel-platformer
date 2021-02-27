cmake_minimum_required(VERSION 3.7)
project(voxel-platformer VERSION 0.1.0 LANGUAGES CXX)

set (SOURCE_FILES
	"src/game/entry.cpp"
	"src/game/config.hpp"
	"src/game/config.cpp"
)
	
add_executable(game ${SOURCE_FILES})
set_target_properties(game PROPERTIES
	CXX_STANDARD 17
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

target_compile_definitions(game
	PRIVATE
		GAME_VERSION="${PROJECT_VERSION}"
		GAME_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
		GAME_VERSION_MINOR=${PROJECT_VERSION_MINOR}
		GAME_VERSION_PATCH=${PROJECT_VERSION_PATCH}
		GLEW_STATIC
)

target_include_directories(game
	PRIVATE
		"src/"
)

# Dependencies
find_package(GLEW REQUIRED)
target_link_libraries(game PRIVATE GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(game PRIVATE OpenGL::GL)

find_package(glm REQUIRED)
target_link_libraries(game PRIVATE glm)

find_package(glfw3 REQUIRED)
target_link_libraries(game PRIVATE glfw)

find_package(freetype CONFIG REQUIRED)
target_link_libraries(game PRIVATE freetype)
