#pragma once

#include <vpg/physics/manifold.hpp>

#include <glm/glm.hpp>

namespace vpg::physics {
    struct AABB {
        glm::vec3 min, max;
    };

    bool aabb_vs_aabb(Manifold& manifold);
    bool aabb_vs_sphere(Manifold& manifold);
}