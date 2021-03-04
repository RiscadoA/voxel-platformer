#pragma once

#include <vpg/ecs/behaviour.hpp>
#include <vpg/data/text.hpp>
#include <vpg/physics/collider.hpp>

using namespace vpg;

struct Turret : public ecs::IBehaviour {
    static constexpr char TypeName[] = "Turret";

    struct Info : public IBehaviour::Info {
        data::Handle<data::Text> bullet;
        float delay, speed;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    Turret(vpg::ecs::Entity entity, const Info& info);
    ~Turret();

    virtual void update(float dt) override;
    void fire();

    data::Handle<data::Text> bullet;
    std::vector<ecs::Entity> bullets;
    int next_bullet;
    ecs::Entity entity;
    float delay, speed;

    float time;
};
