#include <vpg/physics/collider.hpp>
#include <vpg/ecs/transform.hpp>

using namespace vpg;
using namespace vpg::physics;

bool Collider::Info::serialize(memory::Stream& stream) const {
    if (this->type == Type::Sphere) {
        stream.write_comment("Sphere Collider", 0);
        stream.write_string("Sphere");
        stream.write_comment("Radius", 1);
        stream.write_f32(this->sphere.radius);
    }
    else if (this->type == Type::AABB) {
        stream.write_comment("AABB Collider", 0);
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
    std::string str_type = stream.read_string();
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
        for (auto it_b = it_a++; it_b != this->entities.end(); ++it_b) {
            auto col_a = ecs::Coordinator::get_component<Collider>(*it_a);
            auto col_b = ecs::Coordinator::get_component<Collider>(*it_b);

            manifold.a = *it_a;
            manifold.b = *it_b;

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
                this->manifolds.push_back(manifold);
            }
        }
    }
}
