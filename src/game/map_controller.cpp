#include "map_controller.hpp"
#include "manager.hpp"
#include "jumper.hpp"
#include "platform.hpp"
#include "turret.hpp"

#include <vpg/ecs/transform.hpp>

bool MapController::Info::serialize(memory::Stream& stream) const {
    stream.write_ref(this->player);
    stream.write_ref(this->kill_area);
    stream.write_string(this->entry.get_asset()->get_id());
    stream.write_string(this->exit.get_asset()->get_id());
    stream.write_string(this->tutorial.get_asset()->get_id());
    stream.write_string(this->end_message.get_asset()->get_id());
    stream.write_string(this->platform_8.get_asset()->get_id());
    stream.write_string(this->platform_8_32.get_asset()->get_id());
    stream.write_string(this->wall_8_32.get_asset()->get_id());
    stream.write_string(this->turret.get_asset()->get_id());
    stream.write_string(this->base_8_32.get_asset()->get_id());
    stream.write_string(this->grass_16.get_asset()->get_id());

    return !stream.failed();
}

bool MapController::Info::deserialize(memory::Stream& stream) {
    this->player = stream.read_ref();
    this->kill_area = stream.read_ref();
    this->entry = data::Manager::load<data::Text>(stream.read_string());
    this->exit = data::Manager::load<data::Text>(stream.read_string());
    this->tutorial = data::Manager::load<data::Text>(stream.read_string());
    this->end_message = data::Manager::load<data::Text>(stream.read_string());
    this->platform_8 = data::Manager::load<data::Text>(stream.read_string());
    this->platform_8_32 = data::Manager::load<data::Text>(stream.read_string());
    this->wall_8_32 = data::Manager::load<data::Text>(stream.read_string());
    this->turret = data::Manager::load<data::Text>(stream.read_string());
    this->base_32 = data::Manager::load<data::Text>(stream.read_string());
    this->base_8_32 = data::Manager::load<data::Text>(stream.read_string());
    this->grass_16 = data::Manager::load<data::Text>(stream.read_string());
 
    return !stream.failed();
}

MapController::MapController(vpg::ecs::Entity entity, const Info& info) {
    this->tutorial = info.tutorial;
    this->end_message = info.end_message;
    this->platform_8 = info.platform_8;
    this->platform_8_32 = info.platform_8_32;
    this->wall_8_32 = info.wall_8_32;
    this->turret = info.turret;
    this->base_32 = info.base_32;
    this->base_8_32 = info.base_8_32;
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
        auto e = Manager::instance(this->tutorial);
        auto tutorial = ecs::Coordinator::get_component<ecs::Transform>(e);
        tutorial->set_position(glm::vec3(-60.0f, 25.0f, -50.0f));
        tutorial->set_rotation(glm::quat(1.0f, 0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_rotation(glm::angleAxis(glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)));
        grass_16->set_position(glm::vec3(0.0f, 0.0f, -50.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 0.0f, -100.0f));
    }
    else if (this->level_num == 1) {
        auto e = Manager::instance(this->platform_8);
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

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(-95.0f, 0.0f, 0.0f);
        platform->to = glm::vec3(-150.0f, 0.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -120.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->to = glm::vec3(0.0f, 0.0f, 0.0f);
        platform->from = glm::vec3(-55.0f, 0.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -120.0f));
        this->level.push_back(e);
        exit->set_position(glm::vec3(0.0f, 0.0f, -160.0f));
    }
    else if (this->level_num == 2) {
        auto e = Manager::instance(this->platform_8);
        auto platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, 0.0f, -130.0f);
        platform->to = glm::vec3(0.0f, 0.0f, -30.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_position(glm::vec3(0.0f, 0.0f, -160.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, -20.0f, 0.0f);
        platform->to = glm::vec3(0.0f, 100.0f, 0.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, -190.0f));
        this->level.push_back(e);

        e = Manager::instance(this->turret);
        auto turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 0.5f;
        turret->speed = 100.0f;
        transform->set_position(glm::vec3(-40.0f, 10.0f, -60.0f));
        transform->look_at(transform->get_position() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->turret);
        turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 0.5f;
        turret->speed = 100.0f;
        transform->set_position(glm::vec3(40.0f, 10.0f, -100.0f));
        transform->look_at(transform->get_position() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 90.0f, -220.0f));
    }
    else if (this->level_num == 3) {
        auto e = Manager::instance(this->platform_8_32);
        auto platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, 0.0f, -120.0f);
        platform->to = glm::vec3(0.0f, 0.0f, -60.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8_32);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, 0.0f, -210.0f);
        platform->to = glm::vec3(0.0f, 0.0f, -270.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->wall_8_32);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(-60.0f, 0.0f, 0.0f);
        platform->to = glm::vec3(60.0f, 0.0f, 0.0f);
        platform->speed *= 2.0f;
        platform->set_center(glm::vec3(0.0f, 15.0f, -120.0f));
        this->level.push_back(e);

        e = Manager::instance(this->wall_8_32);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(60.0f, 0.0f, 0.0f);
        platform->to = glm::vec3(-60.0f, 0.0f, 0.0f);
        platform->speed *= 2.0f;
        platform->set_center(glm::vec3(0.0f, 15.0f, -210.0f));
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_position(glm::vec3(0.0f, 0.0f, -330.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, -5.0f, 0.0f);
        platform->to = glm::vec3(0.0f, 5.0f, 0.0f);
        platform->speed *= 5.0f;
        platform->set_center(glm::vec3(0.0f, 0.0f, -360.0f));
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_rotation(glm::angleAxis(glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)));
        grass_16->set_position(glm::vec3(0.0f, 40.0f, -390.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8_32);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, 40.0f, -450.0f);
        platform->to = glm::vec3(0.0f, 40.0f, -600.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->turret);
        auto turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 0.5f;
        turret->speed = 100.0f;
        transform->set_position(glm::vec3(-40.0f, 46.0f, -460.0f));
        transform->look_at(transform->get_position() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->turret);
        turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 0.5f;
        turret->speed = 100.0f;
        transform->set_position(glm::vec3(40.0f, 46.0f, -525.0f));
        transform->look_at(transform->get_position() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->turret);
        turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 0.5f;
        turret->speed = 100.0f;
        transform->set_position(glm::vec3(-40.0f, 46.0f, -590.0f));
        transform->look_at(transform->get_position() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 40.0f, -660.0f));
    }
    else if (this->level_num == 4) {
        auto e = Manager::instance(this->platform_8);
        auto platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, 60.0f, -90.0f);
        platform->to = glm::vec3(0.0f, 0.0f, -30.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        platform->speed *= 2.0f;
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        auto grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_position(glm::vec3(0.0f, 50.0f, -130.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(0.0f, 50.0f, -160.0f);
        platform->to = glm::vec3(0.0f, 110.0f, -220.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        platform->speed *= 2.0f;
        this->level.push_back(e);

        e = Manager::instance(this->grass_16);
        grass_16 = ecs::Coordinator::get_component<ecs::Transform>(e);
        grass_16->set_rotation(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
        grass_16->set_position(glm::vec3(0.0f, 100.0f, -260.0f));
        this->level.push_back(e);

        e = Manager::instance(this->turret);
        auto turret = (Turret*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        auto transform = ecs::Coordinator::get_component<ecs::Transform>(e);
        turret->delay = 3.0f;
        turret->speed = 80.0f;
        transform->set_position(glm::vec3(40.0f, 110.0f, -260.0f));
        transform->look_at(transform->get_position() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->base_32);
        auto base_32 = ecs::Coordinator::get_component<ecs::Transform>(e);
        base_32->set_position(glm::vec3(-300.0f, 50.0f, -260.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(-300.0f, 50.0f, -310.0f);
        platform->to = glm::vec3(-300.0f, 50.0f, -370.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        this->level.push_back(e);

        e = Manager::instance(this->platform_8);
        platform = (Platform*)ecs::Coordinator::get_component<ecs::Behaviour>(e)->get();
        platform->from = glm::vec3(-300.0f, 50.0f, -445.0f);
        platform->to = glm::vec3(-300.0f, 50.0f, -385.0f);
        platform->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(-300.0f, 50.0f, -480.0f));
    }
    else {
        auto e = Manager::instance(this->end_message);
        auto end_msg = ecs::Coordinator::get_component<ecs::Transform>(e);
        end_msg->set_position(glm::vec3(0.0f, 25.0f, -75.0f));
        //end_msg->set_rotation(glm::quat(1.0f, 0.0f, 1.0f, 0.0f));
        this->level.push_back(e);

        exit->set_position(glm::vec3(0.0f, 0.0f, -2000.0f));
    }

    this->player->controller->respawn(this->player->spawn_position);
}
