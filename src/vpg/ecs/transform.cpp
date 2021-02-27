#include <vpg/ecs/transform.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace vpg::ecs;

vpg::ecs::Transform::Transform(Entity parent) {
    this->parent = parent;
    this->position = { 0.0f, 0.0f, 0.0f };
    this->scale = { 1.0f, 1.0f, 1.0f };
    this->rotation = glm::quat();
    this->dirty = true;
}

void Transform::set_position(const glm::vec3& position) {
    this->position = position;
    this->dirty = true;
}

void Transform::set_rotation(const glm::quat& rotation) {
    this->rotation = rotation;
    this->dirty = true;
}

void Transform::set_scale(const glm::vec3& scale) {
    this->scale = scale;
    this->dirty = true;
}

glm::mat4 Transform::get_global() {
    if (this->parent == NullEntity) {
        return this->get_local();
    }
    else {
        auto parent = Coordinator::get_component<Transform>(this->parent);
        if (parent == nullptr) {
            this->parent = NullEntity;
            return this->get_local();
        }

        return parent->get_global() * this->get_local();
    }
}

const glm::mat4& Transform::get_local() {
    if (this->dirty) {
        this->update();
    }

    return this->local;
}

void Transform::update() {
    this->local = glm::mat4(1.0f);
    this->local = glm::scale(this->local, this->scale);
    this->local = glm::toMat4(this->get_rotation()) * this->local;
    this->local = glm::translate(this->local, this->position);
    this->dirty = false;
}
