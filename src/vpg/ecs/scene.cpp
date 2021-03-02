#include <vpg/ecs/scene.hpp>
#include <vpg/ecs/transform.hpp>

using namespace vpg::ecs;

Scene::Scene() {
    this->signature = 0; // Every entity is in this system
}

bool Scene::deserialize(memory::Stream& stream) {
    this->clean();

    uint32_t count = stream.read_u32();
    stream.clear_ref_map();
    for (uint32_t i = 0; i < count; ++i) {
        stream.add_ref_map(Coordinator::create_entity(), (int64_t)i);
    }

    while (count--) {
        Entity entity = (Entity)stream.read_ref();
        uint32_t component_count = stream.read_u32();
        while (component_count--) {
            if (!Coordinator::add_component(entity, stream)) {
                this->clean();
                return false;
            }
        }

        /*if (this->deserialize_entity(stream) == NullEntity || stream.failed()) {
            this->clean();
            return false;
        }*/
    }
    
    return true;
}

Entity Scene::deserialize_tree(memory::Stream& stream) {
    uint32_t count = stream.read_u32();
    stream.clear_ref_map();
    for (uint32_t i = 0; i < count; ++i) {
        stream.add_ref_map(Coordinator::create_entity(), (int64_t)i);
    }

    Entity root = NullEntity;

    for (uint32_t index = 0; index < count; ++index) {
        Entity entity = (Entity)stream.ref_read_to_write((int64_t)index);

        uint32_t component_count = stream.read_u32();
        while (component_count--) {
            if (!Coordinator::add_component(entity, stream)) {
                std::cerr << "vpg::ecs::Scene::deserialize_tree() failed:\n"
                          << "Couldn't add component to entity\n";
                for (uint32_t i = 0; i < count; ++i) {
                    Coordinator::destroy_entity((Entity)stream.ref_read_to_write(i));
                }
                return NullEntity;
            }
        }

        auto transform = Coordinator::get_component<Transform>(entity);
        if (transform == nullptr) {
            std::cerr << "vpg::ecs::Scene::deserialize_tree() failed:\n"
                      << "All entities in a tree must have transforms\n";
            for (uint32_t i = 0; i < count; ++i) {
                Coordinator::destroy_entity((Entity)stream.ref_read_to_write(i));
            }
            return NullEntity;
        }

        if (transform->get_parent() == NullEntity) {
            if (root == NullEntity) {
                root = entity;
            }
            else {
                std::cerr << "vpg::ecs::Scene::deserialize_tree() failed:\n"
                          << "A tree must have only one root\n";
                for (uint32_t i = 0; i < count; ++i) {
                    Coordinator::destroy_entity((Entity)stream.ref_read_to_write(i));
                }
                return NullEntity;
            }
        }
    }
    
    return root;
}

void Scene::clean() {
    for (auto it = this->entities.begin(); it != this->entities.end();) {
        Entity e = *(it++);
        Coordinator::destroy_entity(e);
    }
}

