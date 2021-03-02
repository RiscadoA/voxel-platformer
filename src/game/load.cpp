#include <vpg/ecs/scene.hpp>
#include <vpg/data/text.hpp>
#include <vpg/memory/text_stream.hpp>
#include <vpg/memory/string_stream_buffer.hpp>

#include "camera_controller.hpp"
#include "player_controller.hpp"

using namespace vpg;
using namespace vpg::ecs;

bool load_game(Scene* scene) {
    Behaviour::register_type<CameraController>();
    Behaviour::register_type<PlayerController>();

    auto stream_buf = memory::StringStreamBuffer(data::Manager::load<data::Text>("scene.test")->get_content());
    auto stream = memory::TextStream(&stream_buf);
    return scene->deserialize(stream);
}