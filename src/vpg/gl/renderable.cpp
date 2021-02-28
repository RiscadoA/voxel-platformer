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

void vpg::gl::Renderable::serialize(std::ostream& os) {
    os << (int)this->type;
    switch (this->type) {
    case Type::Model:
        os << this->model.get_asset()->get_id() << '\n';
        break;
    }
}

void vpg::gl::Renderable::deserialize(std::istream& is) {
    int type;
    std::string str;
    is >> type;
    this->type = (Type)type;
    switch (this->type) {
    case Type::Model:
        is >> str;
        this->model = data::Manager::load<data::Model>(str);
        break;
    default:
        std::cerr << "vpg::gl::Renderable::deserialize() failed:\n"
                  << "Unknown renderable type " << type << '\n';
        break;
    }
}

RenderableSystem::RenderableSystem() {
    this->signature.set(ecs::Coordinator::get_component_type<Renderable>());
}
