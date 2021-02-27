#pragma once

#include <glm/glm.hpp>

namespace vpg::gl {
    class Camera {
    public:
        Camera(float fov, float aspect_ratio, float z_near, float z_far, glm::vec3 position, glm::vec2 rotation);
        ~Camera() = default;

        void move(const glm::vec3& translation);
        void rotate(const glm::vec2& rotation);

        void set_position(const glm::vec3& position);
        void set_rotation(const glm::vec2& rotation);
        void set_direction(const glm::vec3& dir);
        void look_at(const glm::vec3& point);

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

        inline const glm::vec3& get_position() const { return this->pos; }
        inline const glm::vec2& get_rotation() const { return this->rot; }
        inline const glm::mat4& get_projection() const { return this->proj; }
        inline const glm::mat4& get_view() const { return this->view; }

        inline const glm::vec3& get_forward() const { return this->forward; }
        inline const glm::vec3& get_right() const { return this->right; }
        inline const glm::vec3& get_up() const { return this->up; }
        inline glm::vec3 get_world_up() const { return glm::vec3(0.0f, 1.0f, 0.0f); }

    private:
        float aspect_ratio;
        float fov;
        float z_near, z_far;

        glm::mat4 view;
        glm::mat4 proj;

        glm::vec3 pos, forward, right, up;
        glm::vec2 rot;

        glm::vec4 frustum_planes[6];
    };
}