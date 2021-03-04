#include "player_controller.hpp"
#include "platform.hpp"
#include "bullet.hpp"

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>
#include <vpg/config.hpp>

#include <vpg/ecs/transform.hpp>
#include <vpg/ecs/scene.hpp>
#include <vpg/physics/collider.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

using input::Keyboard;
using input::Mouse;
using Key = Keyboard::Key;

bool PlayerController::Info::serialize(memory::Stream& stream) const {
    stream.write_ref(this->torso);
    stream.write_ref(this->lfoot);
    stream.write_ref(this->rfoot);
    stream.write_ref(this->lhand);
    stream.write_ref(this->rhand);
    stream.write_ref(this->feet_collider);
    return !stream.failed();
}

bool PlayerController::Info::deserialize(memory::Stream& stream) {
    this->torso = stream.read_ref();
    this->lfoot = stream.read_ref();
    this->rfoot = stream.read_ref();
    this->lhand = stream.read_ref();
    this->rhand = stream.read_ref();
    this->feet_collider = stream.read_ref();
    return !stream.failed();
}

PlayerController::PlayerController(ecs::Entity entity, const Info& info) {
    this->entity = entity;
    this->torso = info.torso;
    this->lfoot = info.lfoot;
    this->rfoot = info.rfoot;
    this->lhand = info.lhand;
    this->rhand = info.rhand;
    this->feet_collider = info.feet_collider;

    this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->last_mouse = glm::vec2(INFINITY, INFINITY);
    this->time = 0.0f;
    
    this->camera_distance = 60.0f;
    this->camera_x = glm::pi<float>() / 4.0f;
    this->camera_y = glm::pi<float>() / 4.0f;
    this->on_floor = false;
    this->respawned = false;

    this->mouse_move_listener = Mouse::Move.add_listener(std::bind(
        &PlayerController::mouse_move_callback,
        this,
        std::placeholders::_1
    ));
    this->mouse_scroll_listener = Mouse::Scroll.add_listener(std::bind(
        &PlayerController::mouse_scroll_callback,
        this,
        std::placeholders::_1,
        std::placeholders::_2
    ));
    this->sensitivity = (float)Config::get_float("camera.sensitivity", 0.001);
    Mouse::set_mode(Mouse::Mode::Disabled);

    auto collider = ecs::Coordinator::get_component<physics::Collider>(this->feet_collider);
    collider->on_collision.add_listener(std::bind(
        &PlayerController::on_feet_collision,
        this,
        std::placeholders::_1
    ));

    collider = ecs::Coordinator::get_component<physics::Collider>(this->entity);
    collider->on_collision.add_listener(std::bind(
        &PlayerController::on_body_collision,
        this,
        std::placeholders::_1
    ));

    this->torso_pos = ecs::Coordinator::get_component<ecs::Transform>(this->torso)->get_position();
    this->lfoot_pos = ecs::Coordinator::get_component<ecs::Transform>(this->lfoot)->get_position();
    this->rfoot_pos = ecs::Coordinator::get_component<ecs::Transform>(this->rfoot)->get_position();
    this->lhand_pos = ecs::Coordinator::get_component<ecs::Transform>(this->lhand)->get_position();
    this->rhand_pos = ecs::Coordinator::get_component<ecs::Transform>(this->rhand)->get_position();
}

PlayerController::~PlayerController() {
    Mouse::Move.remove_listener(this->mouse_move_listener);
    Mouse::Move.remove_listener(this->mouse_scroll_listener);
    Mouse::set_mode(Mouse::Mode::Normal);
}

void PlayerController::update(float dt) {
    this->respawned = false;

    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    auto camera = ecs::Coordinator::get_component<ecs::Transform>(this->camera);
    auto torso = ecs::Coordinator::get_component<ecs::Transform>(this->torso);
    auto lfoot = ecs::Coordinator::get_component<ecs::Transform>(this->lfoot);
    auto rfoot = ecs::Coordinator::get_component<ecs::Transform>(this->rfoot);
    auto lhand = ecs::Coordinator::get_component<ecs::Transform>(this->lhand);
    auto rhand = ecs::Coordinator::get_component<ecs::Transform>(this->rhand);

    auto center = glm::vec3(0.0f, 5.0f, 0.0f);
    camera->set_position(transform->get_position() + center + glm::normalize(glm::vec3(
        cos(camera_x) * cos(camera_y),
        sin(camera_y),
        sin(camera_x) * cos(camera_y)
    )) * this->camera_distance);
    camera->look_at(transform->get_position() + center, glm::vec3(0.0f, 1.0f, 0.0f));

    if (!this->on_floor) {
        this->velocity.y -= 98.1 * dt;
    }

    glm::vec2 input = { 0.0f, 0.0f };

    float speed = 30.0f;
    const float jump_change_force = 30.0f;

    if (Keyboard::is_key_pressed(Key::LShift)) {
        speed = 50.0f;
    }

    if (Keyboard::is_key_pressed(Key::W)) {
        input.y = 1.0f;
    }
    else if (Keyboard::is_key_pressed(Key::S)) {
        input.y = -1.0f;
    }

    if (Keyboard::is_key_pressed(Key::D)) {
        input.x = 1.0f;
    }
    else if (Keyboard::is_key_pressed(Key::A)) {
        input.x = -1.0f;
    }

    if (input.x == 0.0f && input.y == 0.0f) {
        this->time += dt * 5.0f;
        this->time = glm::clamp(this->time, 0.0f, 1.0f);
        torso->set_position(glm::mix(torso->get_position(), this->torso_pos, time));
        lfoot->set_position(glm::mix(lfoot->get_position(), this->lfoot_pos, time));
        rfoot->set_position(glm::mix(rfoot->get_position(), this->rfoot_pos, time));
        lhand->set_position(glm::mix(lhand->get_position(), this->lhand_pos, time));
        rhand->set_position(glm::mix(rhand->get_position(), this->rhand_pos, time));
        if (this->on_floor) {
            this->velocity = glm::mix(this->velocity, this->floor_velocity, 30.0f * dt);
        }
    }
    else {
        glm::vec3 desired_dir = camera->get_right() * input.x + camera->get_forward() * input.y;
        desired_dir.y = 0.0f;
        desired_dir = glm::normalize(desired_dir);
        transform->look_at(transform->get_global_position() + glm::mix(
            transform->get_forward(),
            -desired_dir,
            dt * 50.0f
        ), glm::vec3(0.0f, 1.0f, 0.0f));

        if (this->on_floor) {
            this->time -= dt * speed;
            this->time = glm::mod(this->time + 2 * glm::pi<float>(), 2 * glm::pi<float>()) - 2 * glm::pi<float>();
            glm::vec3 desired_torso = this->torso_pos + glm::vec3(0.0f, sin(this->time), 0.0f) * 0.5f;
            glm::vec3 desired_lfoot = this->lfoot_pos + glm::vec3(0.0f, sin(this->time) + 1.0f, cos(this->time)) * 1.0f;
            glm::vec3 desired_rfoot = this->rfoot_pos + glm::vec3(0.0f, sin(this->time + glm::pi<float>()) + 1.0f, cos(this->time + glm::pi<float>())) * 1.0f;
            glm::vec3 desired_lhand = this->lhand_pos + glm::vec3(0.0f, sin(this->time + glm::pi<float>()) + 1.0f, cos(this->time + glm::pi<float>())) * 1.0f;
            glm::vec3 desired_rhand = this->rhand_pos + glm::vec3(0.0f, sin(this->time) + 1.0f, cos(this->time)) * 1.0f;
            torso->set_position(glm::mix(torso->get_position(), desired_torso, dt * 10.0f));
            lfoot->set_position(glm::mix(lfoot->get_position(), desired_lfoot, dt * 10.0f));
            rfoot->set_position(glm::mix(rfoot->get_position(), desired_rfoot, dt * 10.0f));
            lhand->set_position(glm::mix(lhand->get_position(), desired_lhand, dt * 10.0f));
            rhand->set_position(glm::mix(rhand->get_position(), desired_rhand, dt * 10.0f));

            this->velocity = glm::mix(this->velocity, this->floor_velocity + desired_dir * speed, 25.0f * dt);
        }
        else {
            if (glm::dot(this->velocity, desired_dir) < 0.0f) {
                this->velocity += desired_dir * jump_change_force * dt;
            }
        }
    }

    if (Keyboard::is_key_pressed(Key::Space) && this->on_floor) {
        this->velocity.y = 50.0f;
    }

    this->on_floor = false;
    transform->translate(this->velocity * dt);
}

void PlayerController::on_feet_collision(const physics::Manifold& manifold) {
    if (this->velocity.y < 0.0f && !this->on_floor && !this->respawned) {
        this->floor_velocity = { 0.0f, 0.0f, 0.0f };

        bool was_bullet = false;
        auto behaviour = ecs::Coordinator::get_component<ecs::Behaviour>(manifold.a == this->entity ? manifold.b : manifold.a);
        if (behaviour != nullptr) {
            auto platform = dynamic_cast<Platform*>(behaviour->get());
            if (platform != nullptr) {
                this->floor_velocity = platform->velocity;
            }

            auto bullet = dynamic_cast<Bullet*>(behaviour->get());
            if (bullet != nullptr) {
                this->velocity += manifold.normal * bullet->speed * 2.0f;
                glm::vec3 t = { 0.0f, 0.0f, 0.0f };
                t.x = (float)(rand() % 100) / 50.0f - 1.0f;
                t.z = (float)(rand() % 100) / 50.0f - 1.0f;
                t = glm::normalize(t);
                this->velocity += t * bullet->speed * 2.0f;
                was_bullet = true;
            }
        }

        auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
        transform->translate(manifold.normal * manifold.penetration);

        if (!was_bullet) {
            this->velocity.y = 0.0f;
            this->on_floor = true;
        }
    }
}

void PlayerController::on_body_collision(const physics::Manifold& manifold) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    transform->translate(manifold.normal * manifold.penetration);

    auto behaviour = ecs::Coordinator::get_component<ecs::Behaviour>(manifold.a == this->entity ? manifold.b : manifold.a);
    bool was_bullet = false;
    if (behaviour != nullptr) {
        auto bullet = dynamic_cast<Bullet*>(behaviour->get());
        if (bullet != nullptr) {
            this->velocity += manifold.normal * bullet->speed * 2.0f;
            this->velocity.y += bullet->speed * 2.0f;
            was_bullet = true;
        }
    }

    if (!was_bullet) {
        this->velocity -= manifold.normal * glm::dot(manifold.normal, this->velocity);
    }
}

void PlayerController::mouse_move_callback(glm::vec2 mouse) {
    if (this->last_mouse.x != INFINITY) {
        auto delta = this->last_mouse - mouse;
        this->camera_x -= delta.x * this->sensitivity;
        this->camera_y -= delta.y * this->sensitivity;
        this->camera_y = glm::clamp(this->camera_y, 0.1f, glm::half_pi<float>() - 0.1f);
    }

    this->last_mouse = mouse;
}

void PlayerController::mouse_scroll_callback(Mouse::Wheel wheel, float delta) {
    this->camera_distance -= delta * 10.0f;
    this->camera_distance = glm::clamp(this->camera_distance, 15.0f, 100.0f);
}

void PlayerController::respawn(glm::vec3 position) {
    auto transform = ecs::Coordinator::get_component<ecs::Transform>(this->entity);
    transform->set_position(position);
    this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->on_floor = false;
    this->respawned = true;
}
