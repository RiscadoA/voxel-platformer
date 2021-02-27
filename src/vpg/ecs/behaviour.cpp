#include <vpg/ecs/behaviour.hpp>

using namespace vpg::ecs;

vpg::ecs::Behaviour::Behaviour() {
    this->behaviour = nullptr;
}

Behaviour::Behaviour(Behaviour&& rhs) noexcept {
    this->behaviour = rhs.behaviour;
    rhs.behaviour = nullptr;
}

Behaviour::~Behaviour() {
    if (this->behaviour != nullptr) {
        delete this->behaviour;
    }
}

void vpg::ecs::Behaviour::update(float dt) {
    if (this->behaviour != nullptr) {
        this->behaviour->update(dt);
    }
}

BehaviourSystem::BehaviourSystem() {
    this->signature.set(Coordinator::get_component_type<Behaviour>());
}

void BehaviourSystem::update(float dt) {
    for (auto& entity : this->entities) {
        auto& behaviour = *Coordinator::get_component<Behaviour>(entity);
        behaviour.update(dt);
    }
}
