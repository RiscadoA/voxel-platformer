#pragma once

#include <vpg/ecs/behaviour.hpp>
#include <vpg/data/text.hpp>
#include <vpg/physics/collider.hpp>

using namespace vpg;

struct Jumper : public ecs::IBehaviour {
    static constexpr char TypeName[] = "Jumper";

    struct Info : public IBehaviour::Info {
        float bounciness;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    Jumper(vpg::ecs::Entity entity, const Info& info);

    ecs::Entity entity;
    float bounciness;
};
