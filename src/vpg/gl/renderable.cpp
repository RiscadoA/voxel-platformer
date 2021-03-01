#include <vpg/gl/renderable.hpp>

using namespace vpg;
using namespace vpg::gl;

bool Renderable::Info::serialize(memory::Stream& stream) const {
    switch (this->type) {
    case Type::Model:
        stream.write_comment("Model Renderable", 0);
        stream.write_string("Model");
        stream.write_comment("Asset ID", 1);
        stream.write_string(this->model.get_asset()->get_id());
        break;
    default:
        stream.write_comment("Empty Renderable", 0);
        stream.write_string("None");
        break;
    }
    return !stream.failed();
}

bool Renderable::Info::deserialize(memory::Stream& stream) {
    std::string type_str = stream.read_string();
    if (type_str == "Model") {
        this->type = Type::Model;
        std::string asset_id = stream.read_string();
        this->model = data::Manager::load<data::Model>(asset_id);
        if (this->model.get_asset() == nullptr) {
            std::cerr << "vpg::gl::Renderable::Info::deserialize() failed:\n"
                         "No model asset '" << asset_id << "' found\n";
            return false;
        }
    }
    else if (type_str == "None") {
        this->type = Type::None;
    }
    else {
        std::cerr << "vpg::gl::Renderable::Info::deserialize() failed:\n"
                  << "Unknown renderable type " << type_str << '\n';
        return false;
    }

    return true;
}

Renderable::Renderable(ecs::Entity entity, const Info& create_info) {
    this->type = create_info.type;
    switch (this->type) {
    case Renderable::Type::Model:
        new (&this->model) data::Handle<data::Model>(create_info.model);
        break;
    }
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
