#include "player_controller.hpp"

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>
#include <vpg/config.hpp>

#include <vpg/ecs/transform.hpp>
#include <vpg/physics/collider.hpp>

#include <glm/gtc/quaternion.hpp>

using namespace vpg;
using namespace vpg::ecs;

using input::Keyboard;
using input::Mouse;
using Key = Keyboard::Key;

bool PlayerController::Info::serialize(memory::Stream& stream) const {
    return true;
}

bool PlayerController::Info::deserialize(memory::Stream& stream) {
    return true;
}

PlayerController::PlayerController(Entity entity, const Info& info) {
    this->entity = entity;

    auto collider = ecs::Coordinator::get_component<physics::Collider>(entity);
    collider->on_collision.add_listener([this](const physics::Manifold& manifold) {
        this->velocity = -this->velocity;
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
        transform->translate(manifold.normal * manifold.penetration);

        std::cout << "Collision\n";
    });

    this->velocity = false;
}

void PlayerController::update(float dt) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    this->velocity -= 9.81 * dt;

    transform->translate(glm::vec3(0.0f, this->velocity, 0.0f) * dt);
}