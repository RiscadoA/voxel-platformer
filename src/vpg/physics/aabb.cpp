#include <vpg/physics/aabb.hpp>
#include <vpg/physics/collider.hpp>
#include <vpg/ecs/coordinator.hpp>
#include <vpg/ecs/transform.hpp>

using namespace vpg;
using namespace vpg::physics;

bool vpg::physics::aabb_vs_aabb(Manifold& manifold) {
    auto transform_a = ecs::Coordinator::get_component<ecs::Transform>(manifold.a);
    auto transform_b = ecs::Coordinator::get_component<ecs::Transform>(manifold.b);
    auto col_a = ecs::Coordinator::get_component<Collider>(manifold.a);
    auto col_b = ecs::Coordinator::get_component<Collider>(manifold.b);
    assert(transform_a != nullptr && transform_b != nullptr &&
           col_a != nullptr && col_b != nullptr);

    auto center_a = (col_a->aabb.min + col_a->aabb.max) / 2.0f;
    auto center_b = (col_b->aabb.min + col_b->aabb.max) / 2.0f;

    auto n = transform_b->get_global_position() + center_b
           - transform_a->get_global_position() - center_a;

    float a_extent = (col_a->aabb.max.x - col_a->aabb.min.x) / 2.0f;
    float b_extent = (col_b->aabb.max.x - col_b->aabb.min.x) / 2.0f;
    float x_overlap = a_extent + b_extent - fabsf(n.x);
    if (x_overlap <= 0.0f) {
        return false;
    }

    a_extent = (col_a->aabb.max.y - col_a->aabb.min.y) / 2.0f;
    b_extent = (col_b->aabb.max.y - col_b->aabb.min.y) / 2.0f;
    float y_overlap = a_extent + b_extent - fabsf(n.y);
    if (y_overlap < 0.0f) {
        return false;
    }

    a_extent = (col_a->aabb.max.z - col_a->aabb.min.z) / 2.0f;
    b_extent = (col_b->aabb.max.z - col_b->aabb.min.z) / 2.0f;
    float z_overlap = a_extent + b_extent - fabsf(n.z);
    if (z_overlap < 0.0f) {
        return false;
    }

    if (x_overlap < y_overlap && x_overlap < z_overlap) {
        manifold.normal = glm::vec3(n.x < 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
        manifold.penetration = x_overlap;
    }
    else if (y_overlap < x_overlap && y_overlap < z_overlap) {
        manifold.normal = glm::vec3(0.0f, n.y < 0 ? -1.0f : 1.0f, 0.0f);
        manifold.penetration = y_overlap;
    }
    else {
        manifold.normal = glm::vec3(0.0f, 0.0f, n.z < 0 ? -1.0f : 1.0f);
        manifold.penetration = z_overlap;
    }

    return true;
}

bool vpg::physics::aabb_vs_sphere(Manifold& manifold) {
    auto transform_a = ecs::Coordinator::get_component<ecs::Transform>(manifold.a);
    auto transform_b = ecs::Coordinator::get_component<ecs::Transform>(manifold.b);
    auto col_a = ecs::Coordinator::get_component<Collider>(manifold.a);
    auto col_b = ecs::Coordinator::get_component<Collider>(manifold.b);
    assert(transform_a != nullptr && transform_b != nullptr &&
           col_a != nullptr && col_b != nullptr);

    auto center_a = (col_a->aabb.min + col_a->aabb.max) / 2.0f;

    auto n = transform_b->get_global_position() - transform_a->get_global_position() - center_a;
    auto closest = n;

    float x_extent = (col_a->aabb.max.x - col_a->aabb.min.x) / 2.0f;
    float y_extent = (col_a->aabb.max.y - col_a->aabb.min.y) / 2.0f;
    float z_extent = (col_a->aabb.max.z - col_a->aabb.min.z) / 2.0f;

    closest.x = glm::clamp(closest.x, -x_extent, x_extent);
    closest.y = glm::clamp(closest.y, -y_extent, y_extent);
    closest.z = glm::clamp(closest.z, -z_extent, z_extent);

    bool inside = false;

    if (n == closest) {
        inside = true;

        if (fabsf(n.x) > fabsf(n.y) && fabsf(n.x) > fabsf(n.z)) {
            if (closest.x > 0.0f) {
                closest.x = x_extent;
            }
            else {
                closest.x = -x_extent;
            }
        }
        else if (fabsf(n.y) > fabsf(n.x) && fabsf(n.y) > fabsf(n.z)) {
            if (closest.y > 0.0f) {
                closest.y = y_extent;
            }
            else {
                closest.y = -y_extent;
            }
        }
        else {
            if (closest.z > 0.0f) {
                closest.z = z_extent;
            }
            else {
                closest.z = -z_extent;
            }
        }
    }

    auto normal = n - closest;
    auto d_squared = glm::dot(normal, normal);
    auto r = col_b->sphere.radius;

    if (d_squared > r * r && !inside) {
        return false;
    }

    auto d = sqrtf(d_squared);

    if (inside) {
        manifold.normal = -n;
        manifold.penetration = r - d;
    }
    else {
        manifold.normal = n;
        manifold.penetration = r - d;
    }

    return true;
}
