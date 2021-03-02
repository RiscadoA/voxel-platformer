#include "player_controller.hpp"

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>
#include <vpg/config.hpp>

#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/scene.hpp>
#include <vpg/physics/collider.hpp>

#include <glm/gtc/quaternion.hpp>

using input::Keyboard;
using input::Mouse;
using Key = Keyboard::Key;

bool PlayerController::Info::serialize(memory::Stream& stream) const {
    stream.write_ref(this->torso);
    stream.write_ref(this->lfoot);
    stream.write_ref(this->rfoot);
    stream.write_ref(this->lhand);
    stream.write_ref(this->rhand);
    return !stream.failed();
}

bool PlayerController::Info::deserialize(memory::Stream& stream) {
    this->torso = stream.read_ref();
    this->lfoot = stream.read_ref();
    this->rfoot = stream.read_ref();
    this->lhand = stream.read_ref();
    this->rhand = stream.read_ref();
    return !stream.failed();
}

PlayerController::PlayerController(ecs::Entity entity, const Info& info) {
    this->entity = entity;
    this->torso = info.torso;
    this->lfoot = info.lfoot;
    this->rfoot = info.rfoot;
    this->lhand = info.lhand;
    this->rhand = info.rhand;

    //auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->player);
    //transform->set_position(info.position);
}

PlayerController::~PlayerController() {
    ecs::Coordinator::destroy_entity(this->torso);
    ecs::Coordinator::destroy_entity(this->lfoot);
    ecs::Coordinator::destroy_entity(this->rfoot);
    ecs::Coordinator::destroy_entity(this->lhand);
    ecs::Coordinator::destroy_entity(this->rhand);
}