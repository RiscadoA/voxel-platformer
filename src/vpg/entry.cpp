#include <vpg/config.hpp>

#include <vpg/data/manager.hpp>
#include <vpg/data/text.hpp>
#include <vpg/data/shader.hpp>
#include <vpg/data/model.hpp>

#include <vpg/ecs/coordinator.hpp>
#include <vpg/ecs/scene.hpp>
#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/behaviour.hpp>

#include <vpg/memory/string_stream_buffer.hpp>
#include <vpg/memory/text_stream.hpp>

#include <vpg/gl/renderer.hpp>
#include <vpg/gl/debug.hpp>

#include <glm/glm.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <random>

using namespace vpg;

bool load_game(ecs::Scene* scene);

void APIENTRY gl_debug_output(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {

    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
        return;
    }

    std::cerr << "OpenGL debug message (" << id << "): " << message << std::endl;

    switch (source) {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API" << std::endl; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System" << std::endl; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler" << std::endl; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party" << std::endl; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application" << std::endl; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other" << std::endl; break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error" << std::endl; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour" << std::endl; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability" << std::endl; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance" << std::endl; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker" << std::endl; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group" << std::endl; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group" << std::endl; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other" << std::endl; break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: High" << std::endl; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: Medium" << std::endl; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: Low" << std::endl; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: Notification" << std::endl; break;
    }

    std::cout << std::endl;
}

int main(int argc, char** argv) {
    Config::load(argc, argv);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "glfwInit() failed\n";
        return 1;
    }

    // Open window
    glm::ivec2 window_sz = {
        Config::get_integer("window.width", 800),
        Config::get_integer("window.height", 600)
    };

    bool fullscreen = Config::get_boolean("window.fullscreen", false);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(
        window_sz.x,
        window_sz.y,
        "Voxel Platformer Game",
        fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr
    );

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        std::cerr << "glewInit() failed:\n"
                  << glewGetErrorString(glew_status) << '\n';
        return 1;
    }

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // Load assets
    data::Manager::register_type<data::Text>();
    data::Manager::register_type<data::Shader>();
    data::Manager::register_type<data::Model>();
    if (!data::Manager::init()) {
        std::cerr << "Couldn't initialize data manager\n";
        return 1;
    }

    // Init ECS
    ecs::Coordinator::init();
    auto scene = ecs::Coordinator::register_system<ecs::Scene>();
    ecs::Coordinator::register_component<ecs::Transform>();

    // Init behaviour system
    ecs::Coordinator::register_component<ecs::Behaviour>();
    auto behaviour_sys = ecs::Coordinator::register_system<ecs::BehaviourSystem>();

    // Init renderer
    ecs::Coordinator::register_component<gl::Camera>();
    ecs::Coordinator::register_component<gl::Light>();
    ecs::Coordinator::register_component<gl::Renderable>();
    auto camera_sys = ecs::Coordinator::register_system<gl::CameraSystem>();
    auto light_sys = ecs::Coordinator::register_system<gl::LightSystem>();
    auto renderable_sys = ecs::Coordinator::register_system<gl::RenderableSystem>();

    gl::Debug::init();
    auto renderer = new gl::Renderer(window_sz, camera_sys, light_sys, renderable_sys);

    if (!load_game(scene)) {
        std::cerr << "Couldn't load game\n";
        return 1;
    }

    // TODO: Fix delta time
    
    auto last_time = (float)glfwGetTime();
    auto delta_time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Update behaviours
        behaviour_sys->update(delta_time);

        // Render here
        renderer->render(delta_time);

        glfwSwapBuffers(window);

        // Calculate delta time
        auto new_time = (float)glfwGetTime();
        delta_time = new_time - last_time;
        last_time = new_time;
    }

    // Destroy renderer
    delete renderer;
    gl::Debug::terminate();

    // Clean-up ECS
    ecs::Coordinator::terminate();

    // Unload assets
    data::Manager::terminate();

    glfwTerminate();

    return 0;
}