#pragma once

#include <vpg/ecs/scene.hpp>

#include <vpg/data/text.hpp>

using namespace vpg;

class Manager {
public:
    Manager() = delete;
    static bool load();

    static ecs::Entity instance(data::Handle<data::Text> scene);

    static ecs::Scene* scene;
};
