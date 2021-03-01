#include "camera_controller.hpp"

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>
#include <vpg/config.hpp>

#include <vpg/ecs/transform.hpp>

#include <glm/gtc/quaternion.hpp>

using namespace vpg;
using namespace vpg::ecs;

using input::Keyboard;
using input::Mouse;
using Key = Keyboard::Key;

bool CameraController::Info::serialize(memory::Stream& stream) const {
    return true;
}

bool CameraController::Info::deserialize(memory::Stream& stream) {
    return true;
}

CameraController::CameraController(Entity entity, const Info& info) {
    this->entity = entity;
    this->sensitivity = (float)Config::get_float("camera.sensitivity", 0.001);
    this->speed = (float)Config::get_float("camera.speed", 10.0);
    this->last_mouse = glm::vec2(INFINITY, INFINITY);

    Mouse::set_mode(Mouse::Mode::Disabled);
    this->mouse_move_listener = Mouse::Move.add_listener(
        std::bind(&CameraController::mouse_move_callback, this, std::placeholders::_1)
    );
}

CameraController::~CameraController() {
    Mouse::Move.remove_listener(this->mouse_move_listener);
    Mouse::set_mode(Mouse::Mode::Normal);
}

void CameraController::update(float dt) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    if (Keyboard::is_key_pressed(Key::W)) {
        transform->translate(transform->get_forward() * dt * this->speed);
    }
    else if (Keyboard::is_key_pressed(Key::S)) {
        transform->translate(-transform->get_forward() * dt * this->speed);
    }

    if (Keyboard::is_key_pressed(Key::D)) {
        transform->translate(transform->get_right() * dt * this->speed);
    }
    else if (Keyboard::is_key_pressed(Key::A)) {
        transform->translate(-transform->get_right() * dt * this->speed);
    }

    if (Keyboard::is_key_pressed(Key::E)) {
        transform->translate(transform->get_up() * dt * this->speed);
    }
    else if (Keyboard::is_key_pressed(Key::Q)) {
        transform->translate(-transform->get_up() * dt * this->speed);
    }

    if (Keyboard::is_key_pressed(Key::L)) {
        transform->look_at(glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void CameraController::mouse_move_callback(glm::vec2 mouse) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    if (this->last_mouse.x != INFINITY) {
        auto delta = this->last_mouse - mouse;
        glm::quat rot = glm::angleAxis(delta.x * this->sensitivity, glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::angleAxis(delta.y * this->sensitivity, glm::vec3(1.0f, 0.0f, 0.0f));
        transform->rotate(rot);
    }

    this->last_mouse = mouse;
}
