#include <vpg/ecs/coordinator.hpp>

using namespace vpg::ecs;

EntityManager* Coordinator::entity_manager;
ComponentManager* Coordinator::component_manager;
SystemManager* Coordinator::system_manager;

void Coordinator::init() {
    Coordinator::entity_manager = new EntityManager();
    Coordinator::component_manager = new ComponentManager();
    Coordinator::system_manager = new SystemManager();
}

void Coordinator::terminate() {
    delete Coordinator::entity_manager;
    delete Coordinator::component_manager;
    delete Coordinator::system_manager;
}

Entity Coordinator::create_entity() {
    return Coordinator::entity_manager->create();
}

void Coordinator::destroy_entity(Entity entity) {
    Coordinator::entity_manager->destroy(entity);
    Coordinator::component_manager->entity_destroyed(entity);
    Coordinator::system_manager->entity_destroyed(entity);
}

bool Coordinator::add_component(Entity entity, memory::Stream& stream) {
    std::string type;
    if (!Coordinator::component_manager->add_component(entity, stream, type)) {
        return false;
    }
    auto signature = Coordinator::entity_manager->get_signature(entity);
    signature.set(Coordinator::component_manager->get_component_type(type), true);
    Coordinator::entity_manager->set_signature(entity, signature);
    Coordinator::system_manager->entity_signature_changed(entity, signature);
    return true;
}
