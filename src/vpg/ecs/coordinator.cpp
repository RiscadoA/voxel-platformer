#include <vpg/ecs/coordinator.hpp>

using namespace vpg::ecs;

EntityManager Coordinator::entity_manager;
ComponentManager Coordinator::component_manager;
SystemManager Coordinator::system_manager;

Entity Coordinator::create_entity() {
    return Coordinator::entity_manager.create();
}

void Coordinator::destroy_entity(Entity entity) {
    Coordinator::entity_manager.destroy(entity);
    Coordinator::component_manager.entity_destroyed(entity);
    Coordinator::system_manager.entity_destroyed(entity);
}
