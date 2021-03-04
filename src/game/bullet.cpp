#include "bullet.hpp"

#include <vpg/ecs/transform.hpp>

bool Bullet::Info::serialize(memory::Stream& stream) const {
    return !stream.failed();
}

bool Bullet::Info::deserialize(memory::Stream& stream) {
    return !stream.failed();
}

Bullet::Bullet(vpg::ecs::Entity entity, const Info& info) {
    this->entity = entity;
    this->speed = 5.0f;
}

void Bullet::update(float dt) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    transform->translate(transform->get_forward() * this->speed * dt);
}
