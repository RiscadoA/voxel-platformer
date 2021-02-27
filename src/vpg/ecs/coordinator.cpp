#include <vpg/ecs/coordinator.hpp>

using namespace vpg::ecs;

Entity Coordinator::create_entity() {
    return this->entity_manager.create();
}

void Coordinator::destroy_entity(Entity entity) {
    this->entity_manager.destroy(entity);
    this->component_manager.entity_destroyed(entity);
    this->system_manager.entity_destroyed(entity);
}
