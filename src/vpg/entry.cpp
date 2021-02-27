#include <vpg/config.hpp>
#include <vpg/data/manager.hpp>
#include <vpg/data/shader.hpp>
#include <vpg/data/model.hpp>

#include <glm/glm.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace vpg;

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

    // Load assets
    data::Manager::register_type<data::Shader>();
    data::Manager::register_type<data::Model>();
    if (!data::Manager::init()) {
        std::cerr << "Couldn't initialize data manager\n";
        return 1;
    }

    auto lighting = data::Manager::load<data::Shader>("shader.lighting");

    auto shader = data::Manager::load<data::Shader>("shader.mesh");
    auto model = data::Manager::load<data::Model>("model.chr_knight");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Render here
        lighting->get_shader().bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

    // Unload assets
    lighting = nullptr;
    shader = nullptr;
    model = nullptr;
    data::Manager::terminate();

    glfwTerminate();

    return 0;
}