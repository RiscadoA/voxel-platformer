#include "player_instance.hpp"
#include "player_controller.hpp"
#include "manager.hpp"

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>
#include <vpg/config.hpp>

#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/scene.hpp>
#include <vpg/physics/collider.hpp>

#include <vpg/memory/string_stream_buffer.hpp>
#include <vpg/memory/text_stream.hpp>

#include <glm/gtc/quaternion.hpp>

using namespace vpg;

using input::Keyboard;
using input::Mouse;
using Key = Keyboard::Key;

bool PlayerInstance::Info::serialize(memory::Stream& stream) const {
    stream.write_string(this->scene.get_asset()->get_id());
    stream.write_f32(this->position.x);
    stream.write_f32(this->position.y);
    stream.write_f32(this->position.z);
    stream.write_ref(this->camera);
    return !stream.failed();
}

bool PlayerInstance::Info::deserialize(memory::Stream& stream) {
    this->scene = data::Manager::load<data::Text>(stream.read_string());
    this->position.x = stream.read_f32();
    this->position.y = stream.read_f32();
    this->position.z = stream.read_f32();
    this->camera = stream.read_ref();
    return !stream.failed() && this->scene.get_asset() != nullptr;
}

PlayerInstance::PlayerInstance(ecs::Entity entity, const Info& info) {
    this->player = Manager::instance(info.scene);
    this->spawn_position = info.position;

    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->player);
    transform->set_position(info.position);
    this->controller = (PlayerController*)ecs::Coordinator::get_component<ecs::Behaviour>(this->player)->get();
    this->controller->camera = info.camera;
}

PlayerInstance::~PlayerInstance() {
    ecs::Coordinator::destroy_entity(this->player);
}
