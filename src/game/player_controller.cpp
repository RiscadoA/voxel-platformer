#include "player_controller.hpp"

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
using namespace vpg::ecs;

using input::Keyboard;
using input::Mouse;
using Key = Keyboard::Key;

bool PlayerController::Info::serialize(memory::Stream& stream) const {
    stream.write_string(this->scene.get_asset()->get_id());
    stream.write_f32(this->position.x);
    stream.write_f32(this->position.y);
    stream.write_f32(this->position.z);
    return !stream.failed();
}

bool PlayerController::Info::deserialize(memory::Stream& stream) {
    this->scene = data::Manager::load<data::Text>(stream.read_string());
    this->position.x = stream.read_f32();
    this->position.y = stream.read_f32();
    this->position.z = stream.read_f32();
    return !stream.failed() && this->scene.get_asset() != nullptr;
}

PlayerController::PlayerController(Entity entity, const Info& info) {
    auto stream_buf = memory::StringStreamBuffer(info.scene->get_content());
    auto stream = memory::TextStream(&stream_buf);
    this->player = Scene::deserialize_tree(stream);

    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->player);
    transform->set_position(info.position);
}

void PlayerController::update(float dt) {
    
}