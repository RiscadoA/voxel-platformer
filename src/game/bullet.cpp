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
    this->angle = 0.0f;
    this->radius = 0.0f;
    this->last_pos = {};
}

void Bullet::update(float dt) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    switch (this->mode) {
    case Mode::Straight:
        this->velocity = transform->get_forward() * this->speed;
        transform->translate(this->velocity * dt);
        break;
    case Mode::Orbit:
        this->angle += dt * this->speed;
        transform->set_position(this->center + this->radius * glm::vec3(sin(this->angle), 0.0f, cos(this->angle)));
        this->velocity = (transform->get_position() - this->last_pos) / dt;
        this->last_pos = transform->get_position();
        break;
    }
}
