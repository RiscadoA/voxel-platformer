#include <vpg/gl/light.hpp>

using namespace vpg::gl;

LightSystem::LightSystem() {
    this->signature.set(ecs::Coordinator::get_component_type<Light>());
}
