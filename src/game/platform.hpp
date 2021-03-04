#pragma once

#include <vpg/ecs/behaviour.hpp>
#include <vpg/data/text.hpp>
#include <vpg/physics/collider.hpp>

using namespace vpg;

struct Platform : public ecs::IBehaviour {
    static constexpr char TypeName[] = "Platform";

    struct Info : public IBehaviour::Info {
        glm::vec3 from, to;
        float speed;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    Platform(vpg::ecs::Entity entity, const Info& info);

    virtual void update(float dt) override;
    void set_center(const glm::vec3& center);

    ecs::Entity entity;
    glm::vec3 center, from, to;
    glm::vec3 velocity;
    float speed;
};
