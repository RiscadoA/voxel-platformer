#include <vpg/config.hpp>

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Render here

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}