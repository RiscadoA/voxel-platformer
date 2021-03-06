#include <vpg/physics/collider.hpp>
#include <vpg/ecs/transform.hpp>

#include <vpg/gl/debug.hpp>

using namespace vpg;
using namespace vpg::physics;

bool Collider::Info::serialize(memory::Stream& stream) const {
    stream.write_comment("Collider", 0);
    stream.write_string(this->is_static ? "Static" : "Dynamic");
    if (this->type == Type::Sphere) {
        stream.write_string("Sphere");
        stream.write_comment("Radius", 1);
        stream.write_f32(this->sphere.radius);
    }
    else if (this->type == Type::AABB) {
        stream.write_string("AABB");
        stream.write_comment("Min", 1);
        stream.write_f32(this->aabb.min.x);
        stream.write_f32(this->aabb.min.y);
        stream.write_f32(this->aabb.min.z);
        stream.write_comment("Max", 1);
        stream.write_f32(this->aabb.max.x);
        stream.write_f32(this->aabb.max.y);
        stream.write_f32(this->aabb.max.z);
    }
    else {
        std::cerr << "vpg::physics::Collider::Info::serialize() failed:\n"
                  << "Invalid collider type\n";
        return false;
    }

    return !stream.failed();
}

bool Collider::Info::deserialize(memory::Stream& stream) {
    std::string is_static = stream.read_string();
    std::string str_type = stream.read_string();
    if (is_static == "Static") {
        this->is_static = true;
    }
    else if (is_static == "Dynamic") {
        this->is_static = false;
    }
    else {
        std::cerr << "vpg::physics::Collider::Info::deserialize() failed:\n"
                  << "Expected 'Static' or 'Dynamic', found '" << is_static << "'\n";
        return false;
    }

    if (str_type == "Sphere") {
        this->type = Type::Sphere;
        this->sphere.radius = stream.read_f32();
    }
    else if (str_type == "AABB") {
        this->type = Type::AABB;
        this->aabb.min.x = stream.read_f32();
        this->aabb.min.y = stream.read_f32();
        this->aabb.min.z = stream.read_f32();
        this->aabb.max.x = stream.read_f32();
        this->aabb.max.y = stream.read_f32();
        this->aabb.max.z = stream.read_f32();
    }
    else {
        std::cerr << "vpg::physics::Collider::Info::deserialize() failed:\n"
                  << "Unsupported collider type '" << str_type << "'\n";
        return false;
    }

    return !stream.failed();
}

Collider::Collider(ecs::Entity entity, const Info& create_info) {
    this->type = create_info.type;
    this->is_static = create_info.is_static;
    this->sphere = create_info.sphere;
    this->aabb = create_info.aabb;
}

ColliderSystem::ColliderSystem() {
    this->signature.set(ecs::Coordinator::get_component_type<ecs::Transform>());
    this->signature.set(ecs::Coordinator::get_component_type<Collider>());
}

void ColliderSystem::update() {
    this->manifolds.clear();

    Manifold manifold;

    for (auto it_a = this->entities.begin(); it_a != this->entities.end();) {
        auto a = *it_a;
        auto transform_a = ecs::Coordinator::get_component<ecs::Transform>(a);
        auto col_a = ecs::Coordinator::get_component<Collider>(a);

        switch (col_a->type) {
        case Collider::Type::Sphere:
            gl::Debug::draw_sphere(transform_a->get_global_position(), col_a->sphere.radius, { 1.0f, 1.0f, 1.0f, 1.0f });
            break;
        case Collider::Type::AABB:
        {
            auto center = (col_a->aabb.min + col_a->aabb.max) / 2.0f;
            auto scale = (col_a->aabb.max - col_a->aabb.min) / 2.0f;
            gl::Debug::draw_box(transform_a->get_global_position() + center, scale, { 1.0f, 1.0f, 1.0f, 1.0f });
            break;
        }
        }

        for (auto it_b = it_a++; it_b != this->entities.end(); ++it_b) {
            auto b = *it_b;
            if (a == b) {
                continue;
            }

            auto col_b = ecs::Coordinator::get_component<Collider>(b);

            if (col_a->is_static && col_b->is_static) {
                continue;
            }

            manifold.a = a;
            manifold.b = b;

            bool collided;
            if (col_a->type == Collider::Type::Sphere) {
                if (col_b->type == Collider::Type::Sphere) {
                    collided = sphere_vs_sphere(manifold);
                }
                else if (col_b->type == Collider::Type::AABB) {
                    collided = sphere_vs_aabb(manifold);
                }
            }
            else if (col_a->type == Collider::Type::AABB) {
                if (col_b->type == Collider::Type::AABB) {
                    collided = aabb_vs_aabb(manifold);
                }
                else if (col_b->type == Collider::Type::Sphere) {
                    collided = aabb_vs_sphere(manifold);
                }
            }

            if (collided) {
                col_b->on_collision.fire(manifold);
                std::swap(manifold.a, manifold.b);
                manifold.normal = -manifold.normal;
                col_a->on_collision.fire(manifold);
                this->manifolds.push_back(manifold);
            }
        }
    }
}
