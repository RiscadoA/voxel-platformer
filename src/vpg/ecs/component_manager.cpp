#include <vpg/ecs/component_manager.hpp>

using namespace vpg::ecs;

ComponentManager::ComponentManager() {
    this->next_type = 0;
}

ComponentManager::~ComponentManager() {
    for (auto& it : this->arrays) {
        delete it.second;
    }
}

ComponentType ComponentManager::get_component_type(const std::string& name) {
    for (auto& p : this->types) {
        if (p.first == name) {
            return p.second;
        }
    }

    std::cerr << "vpg::ecs::ComponentManager::get_component_type() failed:\n"
              << "Component type '" << name << "'not registered\n";
    abort();
}

bool ComponentManager::add_component(Entity entity, memory::Stream& stream, std::string& type) {
    type = stream.read_string();
    auto it = this->constructors.find(type);
    if (it == this->constructors.end()) {
        std::cerr << "vpg::ecs::ComponentManager::add_component() failed:\n"
                  << "Component type \"" << type << "\" isn't registered\n";
        return false;
    }

    return it->second(entity, stream);
}

void ComponentManager::entity_destroyed(Entity entity) {
    for (auto& it : this->arrays) {
        it.second->entity_destroyed(entity);
    }
}
