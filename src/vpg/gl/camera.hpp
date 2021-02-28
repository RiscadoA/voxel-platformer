#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <glm/glm.hpp>

namespace vpg::gl {
    class Camera : public ecs::Component {
    public:
        static constexpr char TypeName[] = "Camera";

        Camera(
            ecs::Entity entity = ecs::NullEntity,
            float fov = 70.0f,
            float aspect_ratio = 1.0f,
            float z_near = 0.1f,
            float z_far = 1000.0f
        );
        Camera(Camera&&) = default;
        ~Camera() = default;

        void set_fov(float fov);
        void set_aspect_ratio(float aspect_ratio);
        void set_z_near(float z_near);
        void set_z_far(float z_far);

        // Checks if a sphere intersects the camera frustum
        bool intersects_frustum(const glm::vec3& point, float radius) const;

        // Updates the matrices, must be called after changing the camera's transform or properties
        void update();

        inline float get_fov() const { return this->fov; }
        inline float get_aspect_ratio() const { return this->aspect_ratio; }

        inline float get_z_near() const { return this->z_near; }
        inline float get_z_far() const { return this->z_far; }

        inline const glm::mat4& get_view() const { return this->view; }
        inline const glm::mat4& get_proj() const { return this->proj; }

        virtual void serialize(std::ostream& os) override;
        virtual void deserialize(std::istream& is) override;

    private:
        ecs::Entity entity;

        float aspect_ratio;
        float fov;
        float z_near, z_far;

        glm::mat4 view, proj;

        glm::vec4 frustum_planes[6];
    };

    class CameraSystem : public ecs::System {
    public:
        CameraSystem();
    };
}