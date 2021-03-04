#include <vpg/ecs/scene.hpp>
#include <vpg/data/text.hpp>

#include "manager.hpp"
#include "map_controller.hpp"
#include "player_instance.hpp"
#include "player_controller.hpp"
#include "platform.hpp"
#include "turret.hpp"
#include "bullet.hpp"

using namespace vpg;
using namespace vpg::ecs;

bool load_game(Scene* scene) {
    Behaviour::register_type<MapController>();
    Behaviour::register_type<PlayerInstance>();
    Behaviour::register_type<PlayerController>();
    Behaviour::register_type<Platform>();
    Behaviour::register_type<Turret>();
    Behaviour::register_type<Bullet>();

    Manager::scene = scene;
    return Manager::load();
}