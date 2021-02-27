#include <vpg/ecs/entity_manager.hpp>

#include <iostream>

using namespace vpg::ecs;

EntityManager::EntityManager() {
    this->count = 0;
    for (int i = 0; i < MaxEntities; ++i) {
        this->available.push(i);
    }
}

Entity EntityManager::create() {
    this->count += 1;
    if (this->count > MaxEntities) {
        std::cerr << "vpg::ecs::Manager::create() failed:\nToo many entities\n";
        abort(); // Too many entities
    }

    Entity entity = this->available.front();
    this->available.pop();
    return entity;
}

void EntityManager::destroy(Entity entity) {
    this->signatures[entity].reset();
    this->count -= 1;
    this->available.push(entity);
}

void EntityManager::set_signature(Entity entity, Signature signature) {
    this->signatures[entity] = signature;
}

Signature EntityManager::get_signature(Entity entity) {
    return this->signatures[entity];
}