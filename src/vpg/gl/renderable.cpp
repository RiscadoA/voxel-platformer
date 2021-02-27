#include <vpg/gl/renderable.hpp>

using namespace vpg;
using namespace vpg::gl;

Renderable::Renderable() {
    this->type = Renderable::Type::None;
}

Renderable::Renderable(data::Handle<data::Model> model) {
    this->type = Renderable::Type::Model;
    new (&this->model) data::Handle<data::Model>(model);
}

Renderable::Renderable(Renderable&& rhs) noexcept {
    this->type = rhs.type;
    rhs.type = Renderable::Type::None;

    switch (this->type) {
    case Renderable::Type::Model:
        new (&this->model) data::Handle<data::Model>(std::move(rhs.model));
        break;
    }
}

Renderable::~Renderable() {
    switch (this->type) {
    case Renderable::Type::Model:
        this->model.~Handle();
        break;
    }
}

RenderableSystem::RenderableSystem() {
    this->signature.set(ecs::Coordinator::get_component_type<Renderable>());
}
