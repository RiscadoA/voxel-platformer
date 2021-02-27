#include <vpg/ecs/system_manager.hpp>

using namespace vpg::ecs;

void SystemManager::entity_destroyed(Entity entity) {
    for (auto& it : this->systems) {
        it.second->entities.erase(entity);
    }
}

void SystemManager::entity_signature_changed(Entity entity, Signature entity_signature) {
    for (auto& it : this->systems) {
        if ((entity_signature & it.second->signature) == it.second->signature) {
            it.second->entities.insert(entity);
        }
        else {
            it.second->entities.erase(entity);
        }
    }
}
