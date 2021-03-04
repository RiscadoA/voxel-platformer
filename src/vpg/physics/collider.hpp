#pragma once

#include <vpg/ecs/coordinator.hpp>
#include <vpg/data/model.hpp>
#include <vpg/memory/stream.hpp>

#include <vpg/physics/manifold.hpp>
#include <vpg/physics/sphere.hpp>
#include <vpg/physics/aabb.hpp>

#include <vpg/event.hpp>

#include <glm/glm.hpp>

namespace vpg::physics {
    struct Collider {
        static constexpr char TypeName[] = "Collider";

        enum class Type {
            Sphere,
            AABB,
        };

        struct Info {
            Type type;
            bool is_static;
            Sphere sphere;
            AABB aabb;

            bool serialize(memory::Stream& stream) const;
            bool deserialize(memory::Stream& stream);
        };

        Collider(ecs::Entity entity, const Info& create_info);
        Collider(Collider&& rhs) noexcept = default;
        ~Collider() = default;

        Event<const Manifold&> on_collision;
        
        Type type;
        bool is_static;
        Sphere sphere;
        AABB aabb;
    };

    class ColliderSystem : public ecs::System {
    public:
        ColliderSystem();

        void update();
    
        std::vector<Manifold> manifolds;
    };
}