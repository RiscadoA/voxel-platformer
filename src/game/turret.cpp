#include "turret.hpp"
#include "bullet.hpp"
#include "manager.hpp"

#include <vpg/ecs/transform.hpp>

bool Turret::Info::serialize(memory::Stream& stream) const {
    stream.write_string(this->bullet.get_asset()->get_id());
    stream.write_f32(this->delay);
    stream.write_f32(this->speed);

    return !stream.failed();
}

bool Turret::Info::deserialize(memory::Stream& stream) {
    this->bullet = data::Manager::load<data::Text>(stream.read_string());
    this->delay = stream.read_f32();
    this->speed = stream.read_f32();

    return !stream.failed();
}

Turret::Turret(vpg::ecs::Entity entity, const Info& info) {
    this->entity = entity;

    this->bullet = info.bullet;
    this->delay = info.delay;
    this->speed = info.speed;
    this->time = 0.0f;

    this->bullets.resize(16, ecs::NullEntity);
    this->next_bullet = 0;
}

Turret::~Turret() {
    for (int i = 0; i < this->bullets.size(); ++i) {
        if (this->bullets[i] != ecs::NullEntity) {
            Manager::destroy_instance(this->bullets[i]);
        }
    }
}

void Turret::update(float dt) {
    this->time += dt;
    if (this->time > this->delay) {
        this->time -= this->delay;
        this->fire();
    }
}

void Turret::fire() {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);

    if (this->bullets[this->next_bullet] != ecs::NullEntity) {
        ecs::Coordinator::destroy_entity(this->bullets[this->next_bullet]);
    }

    auto e = Manager::instance(this->bullet);
    auto bullet = ecs::Coordinator::get_component<ecs::Transform>(e);
    bullet->set_position(transform->get_global_position());
    bullet->look_at(bullet->get_position() - transform->get_forward(), { 0.0f, 1.0f, 0.0f });
    auto behaviour = (Bullet*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
    behaviour->speed = this->speed;
    this->bullets[this->next_bullet] = e;

    this->next_bullet += 1;
    if (this->next_bullet >= (int)this->bullets.size()) {
        this->next_bullet = 0;
    }
}
