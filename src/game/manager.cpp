#include "manager.hpp"

#include <vpg/memory/text_stream.hpp>
#include <vpg/memory/string_stream_buffer.hpp>

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
