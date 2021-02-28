#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace vpg::ecs {
    class Transform : public Component {
    public:
        static constexpr char TypeName[] = "Transform";

        Transform(Entity parent = NullEntity);

        void translate(const glm::vec3& translation);
        void rotate(const glm::quat& rotation);

        void set_position(const glm::vec3& position);
        void set_rotation(const glm::quat& rotation);
        void set_scale(const glm::vec3& scale);

        void look_at(const glm::vec3& point, const glm::vec3& up);
        
        inline const glm::vec3& get_position() const { return this->position; }
        inline const glm::quat& get_rotation() const { return this->rotation; }
        inline const glm::vec3& get_scale() const { return this->scale; }

        inline const glm::vec3& get_forward() const { return this->forward; }
        inline const glm::vec3& get_right() const { return this->right; }
        inline const glm::vec3& get_up() const { return this->up; }
        
        glm::mat4 get_global();
        const glm::mat4& get_local();

        void update();

        virtual void serialize(std::ostream& os) override;
        virtual void deserialize(std::istream& is) override;

    private:
        Entity parent;
        glm::vec3 position, scale;
        glm::vec3 forward, right, up;
        glm::quat rotation;
        glm::mat4 local;
        bool dirty;
    };
}