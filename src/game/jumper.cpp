#include "jumper.hpp"
#include "player_controller.hpp"

#include <vpg/ecs/transform.hpp>

bool Jumper::Info::serialize(memory::Stream& stream) const {
    stream.write_f32(this->bounciness);
    return !stream.failed();
}

bool Jumper::Info::deserialize(memory::Stream& stream) {
    this->bounciness = stream.read_f32();
    return !stream.failed();
}

Jumper::Jumper(vpg::ecs::Entity entity, const Info& info) {
    this->entity = entity;
    this->bounciness = info.bounciness;
}
