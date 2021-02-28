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
#include <sstream>

using namespace vpg;

GLFWwindow* window;

class CameraBehaviour : public ecs::IBehaviour {
public:
    static constexpr char TypeName[] = "CameraBehaviour";

    static void glfw_cursor_pos_callback(GLFWwindow* win, double x, double y) {
        auto behaviour = CameraBehaviour::current_camera;
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(behaviour->entity);

        static double px = INFINITY, py;
        if (px != INFINITY) {
            glm::quat rot = glm::angleAxis((float)(py - y) * behaviour->sensitivity, glm::vec3(1.0f, 0.0f, 0.0f)) *
                            glm::angleAxis((float)(px - x) * behaviour->sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));
            transform->rotate(rot);
        }

        px = x;
        py = y;
    }

    CameraBehaviour(ecs::Entity entity) {
        this->entity = entity;
        this->sensitivity = 0.001f;
        this->speed = 50.0f;

        this->old_callback = glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        CameraBehaviour::current_camera = this;
    }
    
    ~CameraBehaviour() {
        glfwSetCursorPosCallback(window, this->old_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        CameraBehaviour::current_camera = nullptr;
    }

    virtual void update(float dt) override {
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
  
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            transform->translate(transform->get_forward() * dt * this->speed);
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            transform->translate(-transform->get_forward() * dt * this->speed);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            transform->translate(transform->get_right() * dt * this->speed);
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            transform->translate(-transform->get_right() * dt * this->speed);
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            transform->translate(transform->get_up() * dt * this->speed);
        }
        else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            transform->translate(-transform->get_up() * dt * this->speed);
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            transform->look_at(glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    virtual void serialize(std::ostream& os) override { os << this->sensitivity << this->speed; };
    virtual void deserialize(std::istream& is) override { is >> this->sensitivity >> this->sensitivity; };

private:
    static CameraBehaviour* current_camera;

    ecs::Entity entity;
    GLFWcursorposfun old_callback;

    float sensitivity, speed;
};

CameraBehaviour* CameraBehaviour::current_camera = nullptr;

static void load_test_scene(glm::ivec2 window_sz) {
    ecs::Behaviour::register_type<CameraBehaviour>();

    std::stringstream ss(R"(

    )");

    auto entity = ecs::Coordinator::create_entity();
    auto transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    ecs::Coordinator::add_component<gl::Camera>(entity, gl::Camera(
        entity,
        (float)Config::get_float("camera.fov", 70.0),
        window_sz.x / window_sz.y,
        (float)Config::get_float("camera.near", 0.1),
        (float)Config::get_float("camera.far", 1000.0)
    ));
    ecs::Coordinator::add_component<ecs::Behaviour>(entity, ecs::Behaviour::create<CameraBehaviour>(entity));

    entity = ecs::Coordinator::create_entity();
    transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    transform->set_position(glm::vec3(0.0f, 0.0f, -50.0f));
    auto model = data::Manager::load<data::Model>("model.chr_knight");
    ecs::Coordinator::add_component<gl::Renderable>(entity, gl::Renderable(model));
}

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

    window = glfwCreateWindow(
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
    data::Manager::register_type<data::Shader>();
    data::Manager::register_type<data::Model>();
    if (!data::Manager::init()) {
        std::cerr << "Couldn't initialize data manager\n";
        return 1;
    }

    // Init ECS
    ecs::Coordinator::init();
    ecs::Coordinator::register_component<ecs::Transform>();

    // Init behaviour system
    ecs::Coordinator::register_component<ecs::Behaviour>();
    auto behaviour_sys = ecs::Coordinator::register_system<ecs::BehaviourSystem>();

    // Init renderer
    ecs::Coordinator::register_component<gl::Camera>();
    ecs::Coordinator::register_component<gl::Renderable>();
    auto camera_sys = ecs::Coordinator::register_system<gl::CameraSystem>();
    auto renderable_sys = ecs::Coordinator::register_system<gl::RenderableSystem>();
    auto renderer = new gl::Renderer(window_sz, camera_sys, renderable_sys);

    load_test_scene(window_sz);

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

    // Clean-up ECS
    ecs::Coordinator::terminate();

    // Unload assets
    data::Manager::terminate();

    glfwTerminate();

    return 0;
}