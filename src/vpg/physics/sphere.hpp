#pragma once

#include <vpg/physics/manifold.hpp>

namespace vpg::physics {
    struct Sphere {
        float radius;
    };

    bool sphere_vs_sphere(Manifold& manifold);
    bool sphere_vs_aabb(Manifold& manifold);
}