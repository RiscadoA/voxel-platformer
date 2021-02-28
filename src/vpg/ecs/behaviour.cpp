#include <vpg/ecs/behaviour.hpp>

using namespace vpg::ecs;

std::map<std::string, std::function<IBehaviour*(Entity, std::istream& is)>> Behaviour::registry;

vpg::ecs::Behaviour::Behaviour() {
    this->behaviour = nullptr;
    this->type_name = "null";
    this->entity = NullEntity;
}

Behaviour::Behaviour(Behaviour&& rhs) noexcept {
    this->behaviour = rhs.behaviour;
    this->type_name = rhs.type_name;
    rhs.behaviour = nullptr;
    rhs.type_name = "null";
}

Behaviour::~Behaviour() {
    if (this->behaviour != nullptr) {
        delete this->behaviour;
    }
}

void Behaviour::update(float dt) {
    if (this->behaviour != nullptr) {
        this->behaviour->update(dt);
    }
}

void Behaviour::serialize(std::ostream& os) {
    os << this->type_name;
    if (this->behaviour != nullptr) {
        os << ' ';
        this->behaviour->serialize(os);
    }
}

void Behaviour::deserialize(std::istream& is) {
    is >> this->type_name;
    auto it = this->registry.find(this->type_name);
    if (it == this->registry.end()) {
        std::cerr << "vpg::ecs::Behaviour::deserialize() failed:\n"
                  << "No behaviour type '" << this->type_name << "' registered\n";
        abort();
    }

    if (this->behaviour != nullptr) {
        delete this->behaviour;
    }
    this->behaviour = it->second(this->entity, is);
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
