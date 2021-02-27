#include <vpg/ecs/component_manager.hpp>

vpg::ecs::ComponentManager::ComponentManager() {
    this->next_type = 0;
}

vpg::ecs::ComponentManager::~ComponentManager() {
    for (auto& it : this->arrays) {
        delete it.second;
    }
}

void vpg::ecs::ComponentManager::entity_destroyed(Entity entity) {
    for (auto& it : this->arrays) {
        it.second->entity_destroyed(entity);
    }
}
