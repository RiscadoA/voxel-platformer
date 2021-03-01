#include <vpg/gl/light.hpp>

using namespace vpg::gl;

bool Light::Info::serialize(memory::Stream& stream) const {
    if (this->type == Type::Point) {
        stream.write_comment("Point light", 0);
        stream.write_string("point");
        stream.write_comment("Constant", 1);
        stream.write_f32(this->constant);
        stream.write_comment("Linear", 1);
        stream.write_f32(this->linear);
        stream.write_comment("Quadratic", 1);
        stream.write_f32(this->quadratic);
    }
    else if (this->type == Type::Directional) {
        stream.write_comment("Directional light", 0);
        stream.write_string("directional");
    }
    else {
        std::cerr << "vpg::gl::Light::Info::serialize() failed:\n"
                  << "Invalid light type\n";
        return false;
    }

    stream.write_comment("Ambient", 1);
    stream.write_f32(this->ambient.r);
    stream.write_f32(this->ambient.g);
    stream.write_f32(this->ambient.b);
    stream.write_comment("Diffuse", 1);
    stream.write_f32(this->diffuse.r);
    stream.write_f32(this->diffuse.g);
    stream.write_f32(this->diffuse.b);

    return !stream.failed();
}

bool Light::Info::deserialize(memory::Stream& stream) {
    std::string str_type = stream.read_string();
    if (str_type == "point") {
        this->type = Type::Point;
        this->constant = stream.read_f32();
        this->linear = stream.read_f32();
        this->quadratic = stream.read_f32();
    }
    else if (str_type == "directional") {
        this->type = Type::Directional;
    }
    else {
        std::cerr << "vpg::gl::Light::Info::deserialize() failed:\n"
                  << "Unsupported light type '" << str_type << "'\n";
        return false;
    }

    this->ambient.r = stream.read_f32();
    this->ambient.g = stream.read_f32();
    this->ambient.b = stream.read_f32();
    this->diffuse.r = stream.read_f32();
    this->diffuse.g = stream.read_f32();
    this->diffuse.b = stream.read_f32();

    return !stream.failed();
}

Light::Light(ecs::Entity entity, const Info& create_info) {
    this->type = create_info.type;
    this->ambient = create_info.ambient;
    this->diffuse = create_info.diffuse;
    this->constant = create_info.constant;
    this->linear = create_info.linear;
    this->quadratic = create_info.quadratic;
}

LightSystem::LightSystem() {
    this->signature.set(ecs::Coordinator::get_component_type<Light>());
}
