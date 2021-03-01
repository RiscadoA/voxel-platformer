#include <vpg/config.hpp>

#include <vpg/data/manager.hpp>
#include <vpg/data/shader.hpp>
#include <vpg/data/model.hpp>

#include <vpg/ecs/coordinator.hpp>
#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/behaviour.hpp>

#include <vpg/gl/renderer.hpp>
#include <vpg/gl/debug.hpp>

#include <glm/glm.hpp>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <random>

using namespace vpg;

GLFWwindow* window;

class CameraBehaviour : public ecs::IBehaviour {
public:
    static constexpr char TypeName[] = "CameraBehaviour";

    struct Info : public ecs::IBehaviour::Info {
        virtual bool serialize(memory::Stream& stream) const override {
            return true;
        }

        virtual bool deserialize(memory::Stream& stream) override {
            return true;
        }
    };

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

    CameraBehaviour(ecs::Entity entity, const Info& info) {
        this->entity = entity;
        this->sensitivity = (float)Config::get_float("camera.sensitivity", 10.0);
        this->speed = (float)Config::get_float("camera.speed", 10.0);

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

private:
    static CameraBehaviour* current_camera;

    ecs::Entity entity;
    GLFWcursorposfun old_callback;

    float sensitivity, speed;
};

CameraBehaviour* CameraBehaviour::current_camera = nullptr;

class LightBehaviour : public ecs::IBehaviour {
public:
    static constexpr char TypeName[] = "LightBehaviour";

    struct Info : public ecs::IBehaviour::Info {
        virtual bool serialize(memory::Stream& stream) const override {
            stream.write_comment("Light Behaviour", 0);
            stream.write_comment("Center", 1);
            stream.write_f32(this->center.x);
            stream.write_f32(this->center.y);
            stream.write_f32(this->center.z);
            stream.write_comment("Distance", 1);
            stream.write_f32(this->distance);
            stream.write_comment("Speed", 1);
            stream.write_f32(this->speed);
            return !stream.failed();
        }

        virtual bool deserialize(memory::Stream& stream) override {
            this->center.x = stream.read_f32();
            this->center.y = stream.read_f32();
            this->center.z = stream.read_f32();
            this->distance = stream.read_f32();
            this->speed = stream.read_f32();
            return !stream.failed();
        }

        glm::vec3 center;
        float distance;
        float speed;
    };

    LightBehaviour(ecs::Entity entity, const Info& info) {
        this->entity = entity;
        this->center = info.center;
        this->distance = info.distance;
        this->speed = info.speed;
        this->time = 0.0f;
    }

    virtual void update(float dt) override {
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

        transform->set_position({
            this->center.x + this->distance * sin(this->time + this->center.x + this->center.z),
            this->center.y,
            this->center.z + this->distance * cos(this->time + this->center.x + this->center.z),
        });

        transform->rotate(glm::angleAxis(dt * 5.0f * fabs(sin(this->time)), glm::vec3(0.0f, 1.0f, 0.0f)));

        this->time += dt;
    }

private:
    ecs::Entity entity;
    GLFWcursorposfun old_callback;

    float distance, speed, time;
    glm::vec3 center;
};

static void load_test_scene(glm::ivec2 window_sz) {
    ecs::Behaviour::register_type<CameraBehaviour>();
    ecs::Behaviour::register_type<LightBehaviour>();

    // Initialize camera
    auto camera_entity = ecs::Coordinator::create_entity();
    ecs::Coordinator::add_component<ecs::Transform>(camera_entity, ecs::Transform::Info());
    ecs::Coordinator::add_component<gl::Camera>(camera_entity, gl::Camera::Info{
        (float)Config::get_float("camera.fov", 70.0),
        (float)window_sz.x / (float)window_sz.y,
        (float)Config::get_float("camera.near", 0.1),
        (float)Config::get_float("camera.far", 1000.0)
    });;
    ecs::Coordinator::add_component<ecs::Behaviour>(camera_entity, ecs::Behaviour::Info::create<CameraBehaviour>(CameraBehaviour::Info {}));

    auto model_entity = ecs::Coordinator::create_entity();
    ecs::Coordinator::add_component<ecs::Transform>(model_entity, ecs::Transform::Info {
        ecs::NullEntity,
        glm::vec3(0.0f, 9.0f, -16.0f),
    });
    ecs::Coordinator::add_component<gl::Renderable>(model_entity, gl::Renderable::Info {
        gl::Renderable::Type::Model,
        data::Manager::load<data::Model>("model.dog")
    });

    auto floor_entity = ecs::Coordinator::create_entity();
    ecs::Coordinator::add_component<ecs::Transform>(floor_entity, ecs::Transform::Info {
        ecs::NullEntity,
        glm::vec3(0.0f, -1.0f, 0.0f),
    });
    ecs::Coordinator::add_component<gl::Renderable>(floor_entity, gl::Renderable::Info {
        gl::Renderable::Type::Model,
        data::Manager::load<data::Model>("model.floor")
    });

    srand(time(NULL));
    for (int i = 0; i < 64; ++i) {
        float x = float((rand() % 160) - 80) / 4.0f;
        float z = float((rand() % 160) - 80) / 4.0f;

        auto light_entity = ecs::Coordinator::create_entity();
        ecs::Coordinator::add_component<ecs::Transform>(light_entity, ecs::Transform::Info());
        ecs::Coordinator::add_component<gl::Light>(light_entity, gl::Light::Info {
            gl::Light::Type::Point,
            { 0.0f, 0.0f, 0.0f },
            glm::vec3(float(rand() % 10) / 20.0f + 0.3f, float(rand() % 10) / 20.0f + 0.3f, float(rand() % 10) / 20.0f + 0.3f) * 0.5f,
        });
        LightBehaviour::Info info;
        info.center = glm::vec3(x, 1.0f, z);
        info.distance = float((rand() % 160)) / 16.0f;
        info.speed = 5.0f;
        ecs::Coordinator::add_component<ecs::Behaviour>(light_entity, ecs::Behaviour::Info::create<LightBehaviour>(std::move(info)));
    }

    /*auto entity = ecs::Coordinator::create_entity();
    auto transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    ecs::Coordinator::add_component<gl::Camera>(entity, gl::Camera(
        entity,
        (float)Config::get_float("camera.fov", 70.0),
        (float)window_sz.x / (float)window_sz.y,
        (float)Config::get_float("camera.near", 0.1),
        (float)Config::get_float("camera.far", 1000.0)
    ));
    ecs::Coordinator::add_component<ecs::Behaviour>(entity, ecs::Behaviour::create<CameraBehaviour>(entity));

    entity = ecs::Coordinator::create_entity();
    transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    transform->set_position(glm::vec3(0.0f, 9.0f, -16.0f));
    auto model = data::Manager::load<data::Model>("model.dog");
    ecs::Coordinator::add_component<gl::Renderable>(entity, gl::Renderable(model));

    entity = ecs::Coordinator::create_entity();
    transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    transform->set_position(glm::vec3(0.0f, -1.0f, 0.0f));
    model = data::Manager::load<data::Model>("model.floor");
    ecs::Coordinator::add_component<gl::Renderable>(entity, gl::Renderable(model));

    srand(time(NULL));
    for (int i = 0; i < 64; ++i) {
        float x = float((rand() % 160) - 80) / 4.0f;
        float z = float((rand() % 160) - 80) / 4.0f;
        
        entity = ecs::Coordinator::create_entity();
        transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
        transform->set_scale(glm::vec3(0.1f));
        auto light = &ecs::Coordinator::add_component<gl::Light>(entity, gl::Light());
        light->type = gl::Light::Type::Point;
        light->ambient = { 0.0f, 0.0f, 0.0f };
        light->diffuse = glm::vec3(float(rand() % 10) / 20.0f + 0.3f, float(rand() % 10) / 20.0f + 0.3f, float(rand() % 10) / 20.0f + 0.3f) * 0.5f;
        ecs::Coordinator::add_component<ecs::Behaviour>(entity, ecs::Behaviour::create<LightBehaviour>(entity,
            glm::vec3(x, 1.0f, z),
            float((rand() % 160)) / 16.0f,
            5.0f
        ));
        model = data::Manager::load<data::Model>("model.dog");
        ecs::Coordinator::add_component<gl::Renderable>(entity, gl::Renderable(model));
    }

    /*entity = ecs::Coordinator::create_entity();
    transform = &ecs::Coordinator::add_component<ecs::Transform>(entity, ecs::Transform());
    transform->look_at(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto light = &ecs::Coordinator::add_component<gl::Light>(entity, gl::Light());
    light->type = gl::Light::Type::Directional;
    light->ambient = { 0.2f, 0.2f, 0.2f };
    light->diffuse = { 1.0f, 1.0f, 1.0f };*/
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
    ecs::Coordinator::register_component<gl::Light>();
    ecs::Coordinator::register_component<gl::Renderable>();
    auto camera_sys = ecs::Coordinator::register_system<gl::CameraSystem>();
    auto light_sys = ecs::Coordinator::register_system<gl::LightSystem>();
    auto renderable_sys = ecs::Coordinator::register_system<gl::RenderableSystem>();

    gl::Debug::init();
    auto renderer = new gl::Renderer(window_sz, camera_sys, light_sys, renderable_sys);

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
    gl::Debug::terminate();

    // Clean-up ECS
    ecs::Coordinator::terminate();

    // Unload assets
    data::Manager::terminate();

    glfwTerminate();

    return 0;
}