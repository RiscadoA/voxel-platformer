#pragma once

#include <vpg/ecs/behaviour.hpp>
#include <vpg/data/text.hpp>
#include <vpg/physics/collider.hpp>

#include "player_instance.hpp"
#include "player_controller.hpp"

#include <vector>

using namespace vpg;

struct MapController : public ecs::IBehaviour {
    static constexpr char TypeName[] = "MapController";

    struct Info : public IBehaviour::Info {
        ecs::Entity player, kill_area;
        data::Handle<data::Text> entry, exit, tutorial, end_message;
        data::Handle<data::Text> jumper_8, platform_8, platform_8_32, wall_8_32, turret;
        data::Handle<data::Text> base_32, base_8_32, grass_16;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    MapController(vpg::ecs::Entity entity, const Info& info);
    ~MapController();

    void on_kill_area_collision(const physics::Manifold& manifold);
    void on_exit_area_collision(const physics::Manifold& manifold);
    void gen_level();

    data::Handle<data::Text> tutorial, end_message;
    data::Handle<data::Text> jumper_8, platform_8, platform_8_32, wall_8_32, turret;
    data::Handle<data::Text> base_32, base_8_32, grass_16;

    ecs::Entity kill_area;
    ecs::Entity entry, exit;
    std::vector<ecs::Entity> level;
    PlayerInstance* player;
    int level_num;
};
