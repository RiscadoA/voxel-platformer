#include <vpg/ecs/scene.hpp>
#include <vpg/data/text.hpp>

#include "manager.hpp"
#include "player_instance.hpp"
#include "player_controller.hpp"
#include "map_controller.hpp"
#include "platform.hpp"

using namespace vpg;
using namespace vpg::ecs;

bool load_game(Scene* scene) {
    Behaviour::register_type<Platform>();
    Behaviour::register_type<PlayerInstance>();
    Behaviour::register_type<PlayerController>();
    Behaviour::register_type<MapController>();

    Manager::scene = scene;
    return Manager::load();
}