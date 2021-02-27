#include <vpg/config.hpp>

#include <vpg/data/manager.hpp>
#include <vpg/data/shader.hpp>
#include <vpg/data/model.hpp>

#include <vpg/ecs/coordinator.hpp>
#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/behaviour.hpp>

#include <glm/glm.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace vpg;

class MyBehaviour : public ecs::IBehaviour {
public:
    MyBehaviour(ecs::Entity entity) {
        std::cout << "Initialized MyBehaviour on entity " << entity << '\n';
    }

    ~MyBehaviour() {
        std::cout << "Destroyed MyBehaviour\n";
    }

    virtual void update(float dt) override {
        std::cout << "Updated MyBehaviour with dt " << dt << '\n';
    }
};

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

    // Init Entity-Component System
    ecs::Coordinator::register_component<ecs::Transform>();
    ecs::Coordinator::register_component<ecs::Behaviour>();
    auto behaviour_sys = ecs::Coordinator::register_system<ecs::BehaviourSystem>();

    auto last_time = (float)glfwGetTime();
    auto delta_time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Update behaviours
        behaviour_sys->update(delta_time);

        // Render here
        lighting->get_shader().bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);

        // Calculate delta time
        auto new_time = (float)glfwGetTime();
        delta_time = new_time - last_time;
        last_time = new_time;
    }

    // Unload assets
    lighting = nullptr;
    shader = nullptr;
    model = nullptr;
    data::Manager::terminate();

    glfwTerminate();

    return 0;
}