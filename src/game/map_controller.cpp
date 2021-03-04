#include "map_controller.hpp"
#include "manager.hpp"
#include "platform.hpp"
#include "turret.hpp"

#include <vpg/ecs/transform.hpp>

bool MapController::Info::serialize(memory::Stream& stream) const {
    stream.write_ref(this->player);
    stream.write_ref(this->kill_area);
    stream.write_string(this->entry.get_asset()->get_id());
    stream.write_string(this->exit.get_asset()->get_id());
    stream.write_string(this->tutorial.get_asset()->get_id());
    stream.write_string(this->platform.get_asset()->get_id());
    stream.write_string(this->turret.get_asset()->get_id());
    stream.write_string(this->grass_16.get_asset()->get_id());

    return !stream.failed();
}

bool MapController::Info::deserialize(memory::Stream& stream) {
    this->player = stream.read_ref();
    this->kill_area = stream.read_ref();
    this->entry = data::Manager::load<data::Text>(stream.read_string());
    this->exit = data::Manager::load<data::Text>(stream.read_string());
    this->tutorial = data::Manager::load<data::Text>(stream.read_string());
    this->platform = data::Manager::load<data::Text>(stream.read_string());
    this->turret = data::Manager::load<data::Text>(stream.read_string());
    this->grass_16 = data::Manager::load<data::Text>(stream.read_string());
 
    return !stream.failed();
}

MapController::MapController(vpg::ecs::Entity entity, const Info& info) {
    this->tutorial = info.tutorial;
    this->platform = info.platform;
    this->turret = info.turret;
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

    this->level_num = 2;
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
        auto e = Manager::instance(this->tutorial);
        auto tutorial = ecs::Coordinator::get_component<ecs::Transform>(e);
        tutorial->set_position(glm::vec3(-60.0f, 25.0f, -50.0f));
        tutorial->set_rotation(glm::quat(1.0f, 0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_position(glm::vec3(0.0f, 0.0f, -50.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 0.0f, -100.0f));
    }
    else if (this->level_num == 1) {
        auto e = Manager::instance(this->platform);
        auto platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(25.0f, 0.0f, 0.0f);
        platform->to = glm::vec3(-150.0f, 0.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -40.0f));
        platform->speed = 25.0f;
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_position(glm::vec3(-150.0f, 0.0f, -80.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(-95.0f, 0.0f, 0.0f);
        platform->to = glm::vec3(-150.0f, 0.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -120.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->to = glm::vec3(0.0f, 0.0f, 0.0f);
        platform->from = glm::vec3(-55.0f, 0.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -120.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 0.0f, -160.0f));
    }
    else if (this->level_num == 2) {
        auto e = Manager::instance(this->turret);
        auto turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 5.0f;
        turret->speed = 50.0f;
        transform->set_position(glm::vec3(0.0f, 10.0f, -40.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 0.0f, -160.0f));
    }

    this->player->controller->respawn(this->player->spawn_position);
}
