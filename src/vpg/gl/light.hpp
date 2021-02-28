#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <glm/glm.hpp>

namespace vpg::gl {
    struct Light : public ecs::Component {
        static constexpr char TypeName[] = "Light";

        enum class Type {
            Point,
            Directional,
        } type = Type::Point;

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        glm::vec3 ambient = { 0.0f, 0.0f, 0.0f };
        glm::vec3 diffuse = { 1.0f, 1.0f, 1.0f };

        Light() = default;
        Light(Light&& rhs) noexcept = default;
        ~Light() = default;

        inline virtual void serialize(std::ostream& os) override {};
        inline virtual void deserialize(std::istream& is) override {};
    };

    class LightSystem : public ecs::System {
    public:
        LightSystem();
    };
}