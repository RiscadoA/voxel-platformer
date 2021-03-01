#pragma once

#include <vpg/ecs/coordinator.hpp>
#include <vpg/memory/stream.hpp>

#include <glm/glm.hpp>

namespace vpg::gl {
    struct Light {
        static constexpr char TypeName[] = "Light";

        enum class Type {
            Point,
            Directional,
        };

        struct Info {
            Type type = Type::Point;
            glm::vec3 ambient = { 0.0f, 0.0f, 0.0f };
            glm::vec3 diffuse = { 1.0f, 1.0f, 1.0f };
            float constant = 1.0f;
            float linear = 0.09f;
            float quadratic = 0.032f;

            bool serialize(memory::Stream& stream) const;
            bool deserialize(memory::Stream& stream);
        };

        Light(ecs::Entity entity, const Info& create_info);
        Light(Light&& rhs) noexcept = default;
        ~Light() = default;

        // Data

        Type type;
        float constant;
        float linear;
        float quadratic;
        glm::vec3 ambient;
        glm::vec3 diffuse;
    };

    class LightSystem : public ecs::System {
    public:
        LightSystem();
    };
}