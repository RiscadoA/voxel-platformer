#include <vpg/ecs/transform.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace vpg::ecs;

vpg::ecs::Transform::Transform(Entity parent) {
    this->position = { 0.0f, 0.0f, 0.0f };
    this->scale = { 1.0f, 1.0f, 1.0f };
    this->rotation = glm::quat();
    this->forward = this->rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    this->up = this->rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    this->right = this->rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    this->dirty = true;

    this->parent = NullEntity;
    this->child = NullEntity;
    this->next = NullEntity;

    if (parent != NullEntity) {
        this->set_parent(parent);
    }
}

void vpg::ecs::Transform::translate(const glm::vec3& translation) {
    this->set_position(this->get_position() + translation);
}

void vpg::ecs::Transform::rotate(const glm::quat& rotation) {
    this->set_rotation(this->get_rotation() * rotation);
}

void vpg::ecs::Transform::set_parent(Entity parent) {
    if (this->parent == parent) {
        return;
    }

    if (this->parent != NullEntity) {
        auto p_transform = Coordinator::get_component<Transform>(this->parent);
        if (p_transform != nullptr) {
            auto c = Coordinator::get_component<Transform>(p_transform->child);
            if (c == this) {
                p_transform->child = c->next;
            }
            else {
                for (;;) {
                    auto n = Coordinator::get_component<Transform>(c->next);
                    if (n == this) {
                        c->next = this->next;
                        break;
                    }
                }
            }
        }
    }

    this->parent = parent;

    if (this->parent != NullEntity) {
        auto p_transform = Coordinator::get_component<Transform>(this->parent);
        this->next = p_transform->child;
        p_transform->child = Coordinator::get_component_entity<Transform>(*this);
    }

    this->set_dirty();
}

void Transform::set_position(const glm::vec3& position) {
    this->position = position;
    this->set_dirty();
}

void Transform::set_rotation(const glm::quat& rotation) {
    this->rotation = glm::normalize(rotation);
    this->forward = this->rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    this->up = this->rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    this->right = this->rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    this->set_dirty();
}

void Transform::set_scale(const glm::vec3& scale) {
    this->scale = scale;
    this->set_dirty();
}

void vpg::ecs::Transform::look_at(const glm::vec3& point, const glm::vec3& up) {
    glm::mat4 parent_global = glm::mat4(1.0f);
    if (this->parent != NullEntity) {
        auto parent = Coordinator::get_component<Transform>(this->parent);
        if (parent == nullptr) {
            this->parent = NullEntity;
        }
        parent_global = parent->get_global();
    }

    glm::vec3 local_point = glm::inverse(parent_global) * glm::vec4(point, 1.0f);
    this->set_rotation(glm::quatLookAt(glm::normalize(local_point - this->position), up));
}

const glm::vec3& vpg::ecs::Transform::get_global_position() {
    if (this->dirty) {
        this->update();
    }

    return this->global_position;
}

const glm::quat& vpg::ecs::Transform::get_global_rotation() {
    if (this->dirty) {
        this->update();
    }

    return this->global_rotation;
}

glm::mat4 Transform::get_global() {
    if (this->dirty) {
        this->update();
    }

    return this->global;
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
    this->local = glm::translate(this->local, this->position);
    this->local = this->local * glm::toMat4(this->get_rotation());

    if (this->parent == NullEntity) {
        this->global = this->local;
        this->global_position = this->position;
        this->global_rotation = this->rotation;
    }
    else {
        auto parent = Coordinator::get_component<Transform>(this->parent);
        assert(parent != nullptr);
        this->global = parent->get_global() * this->local;
        this->global_position = this->global * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        this->global_rotation = glm::normalize(parent->global_rotation * this->rotation);
    }

    this->dirty = false;
}

void vpg::ecs::Transform::set_dirty() {
    if (this->dirty) {
        return;
    }

    this->dirty = true;

    if (this->child == NullEntity) {
        return;
    }

    auto c = Coordinator::get_component<Transform>(this->child);
    for (;;) {
        c->set_dirty();
        if (c->next == NullEntity) {
            break;
        }
        c = Coordinator::get_component<Transform>(c->next);
    }
}

void Transform::serialize(std::ostream& os) {
    os << this->parent << '\n';
    os << this->position.x << ' ' << this->position.y << ' ' << this->position.z << '\n';
    os << this->scale.x << ' ' << this->scale.y << ' ' << this->scale.z << '\n';
    os << this->rotation.x << ' ' << this->rotation.y << ' ' << this->rotation.z << ' ' << this->rotation.w;
}

void Transform::deserialize(std::istream& is) {
    glm::vec3 position, scale;
    glm::quat rotation;

    is >> this->parent;
    is >> position.x >> position.y >> position.z;
    is >> scale.x >> scale.y >> scale.z;
    is >> rotation.x >> rotation.y >> rotation.z >> rotation.w;

    this->set_position(position);
    this->set_scale(scale);
    this->set_rotation(rotation);
}
