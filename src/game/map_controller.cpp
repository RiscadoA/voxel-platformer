#include "map_controller.hpp"
#include "manager.hpp"
#include "platform.hpp"

#include <vpg/ecs/transform.hpp>

bool MapController::Info::serialize(memory::Stream& stream) const {
    stream.write_ref(this->player);
    stream.write_ref(this->kill_area);
    stream.write_string(this->entry.get_asset()->get_id());
    stream.write_string(this->exit.get_asset()->get_id());
    stream.write_string(this->platform.get_asset()->get_id());
    stream.write_string(this->grass_16.get_asset()->get_id());

    return !stream.failed();
}

bool MapController::Info::deserialize(memory::Stream& stream) {
    this->player = stream.read_ref();
    this->kill_area = stream.read_ref();
    this->entry = data::Manager::load<data::Text>(stream.read_string());
    this->exit = data::Manager::load<data::Text>(stream.read_string());
    this->platform = data::Manager::load<data::Text>(stream.read_string());
    this->grass_16 = data::Manager::load<data::Text>(stream.read_string());
 
    return !stream.failed();
}

MapController::MapController(vpg::ecs::Entity entity, const Info& info) {
    this->platform = info.platform;
    this->grass_16 = info.grass_16;

    this->kill_area = info.kill_area;
    this->entry = Manager::instance(info.entry);
    this->exit = Manager::instance(info.exit);
    
    auto collider = ecs::Coordinator::get_component<physics::Collider>(info.kill_area);
    collider->on_collision.add_listener(std::bind(
        &MapController::on_kill_area_collision,
        this,
        std::placeholders::_1
    ));

    collider = ecs::Coordinator::get_component<physics::Collider>(this->exit);
    collider->on_collision.add_listener(std::bind(
        &MapController::on_exit_area_collision,
        this,
        std::placeholders::_1
    ));

    this->player = (PlayerInstance*)ecs::Coordinator::get_component<ecs::Behaviour>(info.player)->get();

    this->level_num = 0;
    this->gen_level();
}

MapController::~MapController() {
    ecs::Coordinator::destroy_entity(this->entry);
}

void MapController::on_kill_area_collision(const physics::Manifold& manifold) {
    // Respawn
    this->player->controller->respawn(this->player->spawn_position);
}

void MapController::on_exit_area_collision(const physics::Manifold& manifold) {
    // Respawn
    this->level_num += 1;
    this->gen_level();
    this->player->controller->respawn(this->player->spawn_position);
}

void MapController::gen_level() {
    for (auto& e : this->level) {
        Manager::destroy_instance(e);
    }
    this->level.clear();

    auto exit = ecs::Coordinator::get_component<ecs::Transform>(this->exit);

    if (this->level_num == 0) {
        auto e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_position(glm::vec3(0.0f, 0.0f, -50.0f));
        this->level.push_back(e);
        exit->set_position(glm::vec3(0.0f, 0.0f, -100.0f));
    }
    else if (this->level_num == 1) {
        ecs::Entity e = Manager::instance(this->platform);
        auto platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(-200.0f, 0.0f, 0.0f);
        platform->to = glm::vec3(50.0f, 0.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -40.0f));
        exit->set_position(glm::vec3(-200.0f, 0.0f, -80.0f));
        this->level.push_back(e);
    }

    this->player->controller->respawn(this->player->spawn_position);
}
