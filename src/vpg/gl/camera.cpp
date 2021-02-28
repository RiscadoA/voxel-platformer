#include <vpg/gl/camera.hpp>
#include <vpg/ecs/transform.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace vpg;
using namespace vpg::gl;

Camera::Camera(ecs::Entity entity, float fov, float aspect_ratio, float z_near, float z_far) :
    entity(entity), fov(fov), aspect_ratio(aspect_ratio), z_near(z_near), z_far(z_far) {
    for (int i = 0; i < 6; ++i) {
        this->frustum_planes[i] = glm::vec4(0.0f);
    }
    this->proj = glm::mat4(1.0f);
}

void Camera::set_fov(float fov) {
    this->fov = fov;
}

void Camera::set_aspect_ratio(float aspect_ratio) {
    this->aspect_ratio = aspect_ratio;
}

void Camera::set_z_near(float z_near) {
    this->z_near = z_near;
}

void Camera::set_z_far(float z_far) {
    this->z_far = z_far;
}

bool Camera::intersects_frustum(const glm::vec3& point, float radius) const {
    for (int i = 0; i < 6; ++i) {
        float distance = glm::dot(this->frustum_planes[i], glm::vec4(point, 1.0f)) / glm::length(glm::vec3(this->frustum_planes[i]));
        if (distance + radius < 0) {
            return false;
        }
    }
    return true;
}

void Camera::update() {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    if (transform == nullptr) {
        std::cerr << "vpg::gl:Camera::update() failed:\n"
                  << "A camera component needs a transform component to work\n";
        return;
    }
    
    this->proj = glm::perspective(glm::radians(this->fov), this->aspect_ratio, this->z_near, this->z_far);
    this->view = glm::inverse(transform->get_global());

    // Extract frustum planes
    auto m = glm::transpose(this->proj * this->view);
    this->frustum_planes[0] = m[3] + m[0]; // Left
    this->frustum_planes[1] = m[3] - m[0]; // Right
    this->frustum_planes[2] = m[3] + m[1]; // Bottom
    this->frustum_planes[3] = m[3] - m[1]; // Top
    this->frustum_planes[4] = m[3] + m[2]; // Near
    this->frustum_planes[5] = m[3] - m[2]; // Far
}

void vpg::gl::Camera::serialize(std::ostream& os) {
    os << this->fov << this->aspect_ratio << this->z_near << this->z_far;
}

void vpg::gl::Camera::deserialize(std::istream& is) {
    is >> this->fov >> this->aspect_ratio >> this->z_near >> this->z_far;
}

CameraSystem::CameraSystem() {
    this->signature.set(ecs::Coordinator::get_component_type<Camera>());
}
