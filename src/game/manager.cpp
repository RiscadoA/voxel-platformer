#include "manager.hpp"

#include <vpg/memory/text_stream.hpp>
#include <vpg/memory/string_stream_buffer.hpp>
#include <vpg/ecs/transform.hpp>

ecs::Scene* Manager::scene = nullptr;

bool Manager::load() {
    auto stream_buf = memory::StringStreamBuffer(data::Manager::load<data::Text>("scene.main")->get_content());
    auto stream = memory::TextStream(&stream_buf);
    return Manager::scene->deserialize(stream);
}

ecs::Entity Manager::instance(data::Handle<data::Text> scene) {
    auto stream_buf = memory::StringStreamBuffer(scene->get_content());
    auto stream = memory::TextStream(&stream_buf);
    return ecs::Scene::deserialize_tree(stream);
}

void Manager::destroy_instance(ecs::Entity entity) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(entity);
    auto c = transform->get_child();
    while (c != ecs::NullEntity) {
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(entity);
        auto e = c;
        c = transform->get_next();
        transform->set_parent(ecs::NullEntity);
        ecs::Coordinator::destroy_entity(e);
    }
    ecs::Coordinator::destroy_entity(entity);
}
