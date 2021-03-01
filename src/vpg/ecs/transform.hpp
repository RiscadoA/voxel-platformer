#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace vpg::ecs {
    class Transform {
    public:
        static constexpr char TypeName[] = "Transform";

        struct Info {
            Entity parent = NullEntity;
            glm::vec3 position = {};
            glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
            glm::quat rotation = {};

            bool serialize(memory::Stream& stream) const;
            bool deserialize(memory::Stream& stream);
        };

        Transform(Entity entity, const Info& create_info);

        void translate(const glm::vec3& translation);
        void rotate(const glm::quat& rotation);

        void set_parent(Entity parent);
        void set_position(const glm::vec3& position);
        void set_rotation(const glm::quat& rotation);
        void set_scale(const glm::vec3& scale);

        void look_at(const glm::vec3& point, const glm::vec3& up);
        
        inline const glm::vec3& get_position() const { return this->position; }
        inline const glm::quat& get_rotation() const { return this->rotation; }
        inline const glm::vec3& get_scale() const { return this->scale; }

        const glm::vec3& get_global_position();
        const glm::quat& get_global_rotation();

        inline const glm::vec3& get_forward() const { return this->forward; }
        inline const glm::vec3& get_right() const { return this->right; }
        inline const glm::vec3& get_up() const { return this->up; }
        
        glm::mat4 get_global();
        const glm::mat4& get_local();

        void update();
        void set_dirty();

    private:
        Entity parent, child, next;
        glm::vec3 position, global_position, scale;
        glm::vec3 forward, right, up;
        glm::quat rotation, global_rotation;
        glm::mat4 global, local;
        bool dirty;
    };
}