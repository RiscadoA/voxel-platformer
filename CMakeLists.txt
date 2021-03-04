cmake_minimum_required(VERSION 3.7)
project(voxel-platformer VERSION 0.1.0 LANGUAGES CXX)

set (VPG_SOURCE_FILES
	"src/vpg/entry.cpp"
	"src/vpg/config.hpp"
	"src/vpg/config.cpp"
	"src/vpg/event.hpp"

	"src/vpg/memory/endianness.hpp"
	"src/vpg/memory/pool.hpp"
	"src/vpg/memory/pool.cpp"
	"src/vpg/memory/stream.hpp"
	"src/vpg/memory/stream.cpp" 
	"src/vpg/memory/text_stream.hpp"
	"src/vpg/memory/stream_buffer.hpp"
	"src/vpg/memory/std_stream_buffer.hpp"
	"src/vpg/memory/std_stream_buffer.cpp"
	"src/vpg/memory/text_stream.cpp"
	"src/vpg/memory/string_stream_buffer.hpp"
	"src/vpg/memory/string_stream_buffer.cpp"

	"src/vpg/input/window.hpp"
	"src/vpg/input/window.cpp"
	"src/vpg/input/mouse.hpp"
	"src/vpg/input/mouse.cpp"
	"src/vpg/input/keyboard.cpp"
	"src/vpg/input/keyboard.hpp"

	"src/vpg/gl/camera.hpp"
	"src/vpg/gl/camera.cpp"
	"src/vpg/gl/shader.hpp"
	"src/vpg/gl/shader.cpp"
	"src/vpg/gl/usage.hpp"
	"src/vpg/gl/vertex_buffer.hpp"
	"src/vpg/gl/vertex_buffer.cpp"
	"src/vpg/gl/index_buffer.hpp"
	"src/vpg/gl/index_buffer.cpp"
	"src/vpg/gl/vertex_array.hpp"
	"src/vpg/gl/vertex_array.cpp"
	"src/vpg/gl/light.hpp"
	"src/vpg/gl/light.cpp"
	"src/vpg/gl/debug.hpp"
	"src/vpg/gl/debug.cpp"

	"src/vpg/gl/palette.hpp"
	"src/vpg/gl/palette.cpp"
	"src/vpg/gl/matrix.hpp"
	"src/vpg/gl/matrix.cpp"

	"src/vpg/gl/renderable.hpp"
	"src/vpg/gl/renderable.cpp"
	"src/vpg/gl/renderer.hpp"
	"src/vpg/gl/renderer.cpp"

	"src/vpg/ecs/entity_manager.hpp"
	
	"src/vpg/data/manager.hpp"
	"src/vpg/data/manager.cpp"
	"src/vpg/data/asset.hpp"
	"src/vpg/data/asset.cpp"
	"src/vpg/data/handle.hpp"
	"src/vpg/data/handle.cpp"
	"src/vpg/data/shader.hpp"
	"src/vpg/data/shader.cpp"
	"src/vpg/data/qb_parser.hpp"
	"src/vpg/data/qb_parser.cpp"
	"src/vpg/data/model.hpp"
	"src/vpg/data/model.cpp"
	"src/vpg/data/text.hpp"
	"src/vpg/data/text.cpp"

	"src/vpg/ecs/entity_manager.hpp"
	"src/vpg/ecs/entity_manager.cpp"
	"src/vpg/ecs/component_manager.hpp"
	"src/vpg/ecs/component_manager.cpp"
	"src/vpg/ecs/component_array.hpp"
	"src/vpg/ecs/system_manager.hpp"
	"src/vpg/ecs/system_manager.cpp"
	"src/vpg/ecs/coordinator.hpp"
	"src/vpg/ecs/coordinator.cpp"

	"src/vpg/ecs/scene.hpp"
	"src/vpg/ecs/scene.cpp"
	"src/vpg/ecs/transform.hpp"
	"src/vpg/ecs/transform.cpp"
	"src/vpg/ecs/behaviour.hpp"
	"src/vpg/ecs/behaviour.cpp"
	
	"src/vpg/physics/collider.hpp"
	"src/vpg/physics/collider.cpp"
	"src/vpg/physics/manifold.hpp"
	"src/vpg/physics/sphere.hpp"
	"src/vpg/physics/sphere.cpp"
	"src/vpg/physics/aabb.hpp"
	"src/vpg/physics/aabb.cpp"
)

set (GAME_SOURCE_FILES
	"src/game/load.cpp"
	"src/game/manager.hpp"
	"src/game/manager.cpp"
	
	"src/game/player_controller.hpp"
	"src/game/player_controller.cpp"
	"src/game/player_instance.hpp"
	"src/game/player_instance.cpp"
	"src/game/map_controller.hpp"
	"src/game/map_controller.cpp"

	"src/game/platform.hpp"
	"src/game/platform.cpp"
	"src/game/turret.hpp"
	"src/game/turret.cpp"
	"src/game/bullet.hpp"
	"src/game/bullet.cpp"
)

add_library(vpg ${VPG_SOURCE_FILES})
set_target_properties(vpg PROPERTIES
	CXX_STANDARD 17
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
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
	PUBLIC
		"src/"
)

# Dependencies
find_package(GLEW REQUIRED)
target_link_libraries(vpg PRIVATE GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(vpg PRIVATE OpenGL::GL)

find_package(glm REQUIRED)
target_link_libraries(vpg PUBLIC glm)

find_package(glfw3 REQUIRED)
target_link_libraries(vpg PRIVATE glfw)

find_package(freetype CONFIG REQUIRED)
target_link_libraries(vpg PRIVATE freetype)

add_executable(game ${GAME_SOURCE_FILES})
set_target_properties(game PROPERTIES
	CXX_STANDARD 17
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(game PUBLIC vpg)