#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace vpg::ecs {
    class Transform {
    public:
        Transform(Entity parent = NullEntity);

        void set_position(const glm::vec3& position);
        void set_rotation(const glm::quat& rotation);
        void set_scale(const glm::vec3& scale);
        
        inline const glm::vec3& get_position() const { return this->position; }
        inline const glm::quat& get_rotation() const { return this->rotation; }
        inline const glm::vec3& get_scale() const { return this->scale; }
        
        glm::mat4 get_global();
        const glm::mat4& get_local();

        void update();

    private:
        Entity parent;
        glm::vec3 position, scale;
        glm::quat rotation;
        glm::mat4 local;
        bool dirty;
    };
}