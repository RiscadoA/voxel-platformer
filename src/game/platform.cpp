#include "platform.hpp"
#include "player_controller.hpp"

#include <vpg/ecs/transform.hpp>

bool Platform::Info::serialize(memory::Stream& stream) const {
    stream.write_f32(this->from.x);
    stream.write_f32(this->from.y);
    stream.write_f32(this->from.z);
    stream.write_f32(this->to.x);
    stream.write_f32(this->to.y);
    stream.write_f32(this->to.z);
    stream.write_f32(this->speed);

    return !stream.failed();
}

bool Platform::Info::deserialize(memory::Stream& stream) {
    this->from.x = stream.read_f32();
    this->from.y = stream.read_f32();
    this->from.z = stream.read_f32();
    this->to.x = stream.read_f32();
    this->to.y = stream.read_f32();
    this->to.z = stream.read_f32();
    this->speed = stream.read_f32();

    return !stream.failed();
}

Platform::Platform(vpg::ecs::Entity entity, const Info& info) {
    this->entity = entity;
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    this->center = transform->get_position();

    this->from = info.from;
    this->to = info.to;
    this->speed = info.speed;

    transform->set_position(this->center + this->from);
}

void Platform::update(float dt) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    if (glm::distance(this->center + this->to, transform->get_position()) < this->speed * dt) {
        std::swap(this->to, this->from);
    }
    this->velocity = glm::normalize(this->to - this->from) * this->speed;
    transform->translate(this->velocity * dt);
}

void Platform::set_center(const glm::vec3& center) {
    this->center = center;
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    transform->set_position(this->center + this->from);
}
