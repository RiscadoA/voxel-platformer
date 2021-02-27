#include <vpg/config.hpp>

#include <vpg/data/manager.hpp>
#include <vpg/data/shader.hpp>
#include <vpg/data/model.hpp>

#include <vpg/ecs/coordinator.hpp>
#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/behaviour.hpp>

#include <vpg/gl/renderer.hpp>

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

static void load_test_scene(glm::vec2 window_sz) {
    auto entity = ecs::Coordinator::create_entity();
    auto transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    ecs::Coordinator::add_component<gl::Camera>(entity, gl::Camera(
        entity,
        (float)Config::get_float("camera.fov", 70.0),
        window_sz.x / window_sz.y,
        (float)Config::get_float("camera.near", 0.1),
        (float)Config::get_float("camera.far", 1000.0)
    ));

    entity = ecs::Coordinator::create_entity();
    transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    transform->set_position(glm::vec3(0.0f, 0.0f, -50.0f));
    auto model = data::Manager::load<data::Model>("model.chr_knight");
    ecs::Coordinator::add_component<gl::Renderable>(entity, gl::Renderable(model));
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

    // Init ECS
    ecs::Coordinator::init();
    ecs::Coordinator::register_component<ecs::Transform>();
    ecs::Coordinator::register_component<ecs::Behaviour>();

    // Init behaviour system
    auto behaviour_sys = ecs::Coordinator::register_system<ecs::BehaviourSystem>();

    // Init renderer
    ecs::Coordinator::register_component<gl::Camera>();
    ecs::Coordinator::register_component<gl::Renderable>();
    auto camera_sys = ecs::Coordinator::register_system<gl::CameraSystem>();
    auto renderable_sys = ecs::Coordinator::register_system<gl::RenderableSystem>();
    auto renderer = new gl::Renderer(camera_sys, renderable_sys);

    load_test_scene(window_sz);

    // TODO: Fix delta time
    
    auto last_time = (float)glfwGetTime();
    auto delta_time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Update behaviours
        behaviour_sys->update(delta_time);

        // Render here
        renderer->render();

        glfwSwapBuffers(window);

        // Calculate delta time
        auto new_time = (float)glfwGetTime();
        delta_time = new_time - last_time;
        last_time = new_time;
    }

    // Destroy renderer
    delete renderer;

    // Clean-up ECS
    ecs::Coordinator::terminate();

    // Unload assets
    data::Manager::terminate();

    glfwTerminate();

    return 0;
}