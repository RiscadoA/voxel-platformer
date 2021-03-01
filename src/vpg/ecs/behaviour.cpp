#include <vpg/ecs/behaviour.hpp>

using namespace vpg::ecs;

std::map<std::string, std::function<IBehaviour::Info*()>> Behaviour::info_constructors;
std::map<std::string, std::function<IBehaviour*(Entity, const IBehaviour::Info*)>> Behaviour::constructors;

Behaviour::Info::Info() {
    this->info = nullptr;
    this->name = "null";
}

Behaviour::Info::Info(Info&& rhs) noexcept {
    this->info = rhs.info;
    this->name = rhs.name;
    rhs.info = nullptr;
    rhs.name = "null";
}

Behaviour::Info::~Info() {
    if (this->info != nullptr) {
        delete this->info;
    }
}

bool Behaviour::Info::serialize(memory::Stream& stream) const {
    stream.write_comment("Behaviour", 0);
    stream.write_string(this->name);
    if (this->info != nullptr) {
        if (!this->info->serialize(stream)) {
            return false;
        }
    }
    return !stream.failed();
}

bool Behaviour::Info::deserialize(memory::Stream& stream) {
    if (this->info != nullptr) {
        delete this->info;
    }

    this->name = stream.read_string();
    if (this->name == "null") {
        this->info = nullptr;
        return true;
    }
   
    auto it = Behaviour::info_constructors.find(this->name);
    if (it == Behaviour::info_constructors.end()) {
        std::cerr << "vpg::ecs::Behaviour::Info::deserialize() failed:\n"
                  << "No behaviour '" << this->name << "' registered\n";
        this->info = nullptr;
        this->name = "null";
        return false;
    }

    this->info = it->second();
    if (!this->info->deserialize(stream) || stream.failed()) {
        delete this->info;
        this->info = nullptr;
        this->name = "null";
        return false;
    }

    return true;
}

Behaviour::Behaviour(Entity entity, const Info& info) {
    this->name = info.name;
    auto it = Behaviour::constructors.find(this->name);
    if (it == Behaviour::constructors.end()) {
        std::cerr << "vpg::ecs::Behaviour::Behaviour() failed:\n"
                  << "No behaviour '" << this->name << "' registered\n";
        this->name = "null";
        this->behaviour = nullptr;
    }
    else {
        this->behaviour = it->second(entity, info.info);
    }
}

Behaviour::Behaviour(Behaviour&& rhs) noexcept {
    this->name = rhs.name;
    this->behaviour = rhs.behaviour;
    rhs.name = "null";
    rhs.behaviour = nullptr;
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

BehaviourSystem::BehaviourSystem() {
    this->signature.set(Coordinator::get_component_type<Behaviour>());
}

void BehaviourSystem::update(float dt) {
    for (auto& entity : this->entities) {
        auto& behaviour = *Coordinator::get_component<Behaviour>(entity);
        behaviour.update(dt);
    }
}
