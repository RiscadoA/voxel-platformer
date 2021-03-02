#include <vpg/physics/sphere.hpp>
#include <vpg/physics/collider.hpp>
#include <vpg/ecs/coordinator.hpp>
#include <vpg/ecs/transform.hpp>

using namespace vpg;
using namespace vpg::physics;

bool vpg::physics::sphere_vs_sphere(Manifold& manifold) {
    auto transform_a = ecs::Coordinator::get_component<ecs::Transform>(manifold.a);
    auto transform_b = ecs::Coordinator::get_component<ecs::Transform>(manifold.b);
    auto col_a = ecs::Coordinator::get_component<Collider>(manifold.a);
    auto col_b = ecs::Coordinator::get_component<Collider>(manifold.b);
    assert(transform_a != nullptr && transform_b != nullptr &&
           col_a != nullptr && col_b != nullptr);

    auto n = transform_b->get_global_position() - transform_a->get_global_position();
    float r = col_a->sphere.radius + col_b->sphere.radius;

    auto d_squared = glm::dot(n, n);
    if (d_squared > r * r) {
        return false;
    }

    float d = sqrtf(d_squared);
    if (d != 0) {
        manifold.penetration = r - d;
        manifold.normal = n / d;
    }
    else {
        manifold.penetration = col_a->sphere.radius;
        manifold.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    return true;
}

bool vpg::physics::sphere_vs_aabb(Manifold& manifold) {
    std::swap(manifold.a, manifold.b);
    auto ret = aabb_vs_sphere(manifold);
    std::swap(manifold.a, manifold.b);
    return ret;
}
