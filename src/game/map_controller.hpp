#pragma once

#include <vpg/ecs/behaviour.hpp>
#include <vpg/data/text.hpp>
#include <vpg/physics/collider.hpp>

#include "player_instance.hpp"
#include "player_controller.hpp"

using namespace vpg;

struct MapController : public ecs::IBehaviour {
    static constexpr char TypeName[] = "MapController";

    struct Info : public IBehaviour::Info {
        ecs::Entity player, kill_area;
        data::Handle<data::Text> entry;
        data::Handle<data::Text> platform;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    MapController(vpg::ecs::Entity entity, const Info& info);
    ~MapController();

    void on_kill_area_collision(const physics::Manifold& manifold);

    ecs::Entity kill_area;
    ecs::Entity entry, platform;
    PlayerInstance* player;
};
