#include "map_controller.hpp"
#include "manager.hpp"
#include "platform.hpp"

#include <vpg/ecs/transform.hpp>

bool MapController::Info::serialize(memory::Stream& stream) const {
    stream.write_ref(this->player);
    stream.write_ref(this->kill_area);
    stream.write_string(this->entry.get_asset()->get_id());
    stream.write_string(this->platform.get_asset()->get_id());

    return !stream.failed();
}

bool MapController::Info::deserialize(memory::Stream& stream) {
    this->player = stream.read_ref();
    this->kill_area = stream.read_ref();
    this->entry = data::Manager::load<data::Text>(stream.read_string());
    this->platform = data::Manager::load<data::Text>(stream.read_string());
 
    return !stream.failed();
}

MapController::MapController(vpg::ecs::Entity entity, const Info& info) {
    this->kill_area = info.kill_area;
    this->entry = Manager::instance(info.entry);
    this->platform = Manager::instance(info.platform);

    auto platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(this->platform)->get();
    platform->set_center(glm::vec3(0.0f, 0.0f, 30.0f));

    auto collider = ecs::Coordinator::get_component<physics::Collider>(info.kill_area);
    collider->on_collision.add_listener(std::bind(
        &MapController::on_kill_area_collision,
        this,
        std::placeholders::_1
    ));

    this->player = (PlayerInstance*)ecs::Coordinator::get_component<ecs::Behaviour>(info.player)->get();
}

MapController::~MapController() {
    ecs::Coordinator::destroy_entity(this->entry);
}

void MapController::on_kill_area_collision(const physics::Manifold& manifold) {
    // Respawn
    this->player->controller->respawn(this->player->spawn_position);
}