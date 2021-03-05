#include <vpg/input/window.hpp>
#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>
#include <vpg/config.hpp>

#include <iostream>
#include <GLFW/glfw3.h>

using namespace vpg;
using namespace vpg::input;

Event<glm::ivec2> Window::FramebufferResized;

void* Window::handle = nullptr;

void glfw_framebuffer_resized_callback(GLFWwindow* window, int width, int height) {
    Window::FramebufferResized.fire({ width, height });
}

bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "vpg::input::Window::init():\n"
                  << "glfwInit() failed\n";
        return false;
    }
    
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    bool fullscreen = Config::get_boolean("window.fullscreen", false);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    Window::handle = glfwCreateWindow(
        fullscreen ? mode->width : Config::get_integer("window.width", 640),
        fullscreen ? mode->height : Config::get_integer("window.height", 480),
        Config::get_string("window.title", "RiscadoA's Voxel Platformer").c_str(),
        fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr
    );

    if (Window::handle == nullptr) {
        return false;
    }

    glfwMakeContextCurrent((GLFWwindow*)Window::handle);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback((GLFWwindow*)Window::handle, &glfw_framebuffer_resized_callback);

    Mouse::init();
    Keyboard::init();

    return true;
}

void Window::terminate() {
    glfwTerminate();
}

void* Window::get_handle() {
    return Window::handle;
}

glm::ivec2 vpg::input::Window::get_framebuffer_size() {
    glm::ivec2 ret;
    glfwGetFramebufferSize((GLFWwindow*)Window::handle, &ret.x, &ret.y);
    return ret;
}

bool vpg::input::Window::should_close() {
    return glfwWindowShouldClose((GLFWwindow*)Window::handle);
}

void vpg::input::Window::poll_events() {
    glfwPollEvents();
}

void vpg::input::Window::swap_buffers() {
    glfwSwapBuffers((GLFWwindow*)Window::handle);
}
