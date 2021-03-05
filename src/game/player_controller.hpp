#pragma once

#include <vpg/ecs/behaviour.hpp>

#include <vpg/data/text.hpp>

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>

#include <vpg/physics/collider.hpp>

using namespace vpg;

struct PlayerController : public ecs::IBehaviour {
    static constexpr char TypeName[] = "PlayerController";

    struct Info : public IBehaviour::Info {
        ecs::Entity torso;
        ecs::Entity lfoot, rfoot;
        ecs::Entity lhand, rhand;
        ecs::Entity feet_collider;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    PlayerController(ecs::Entity entity, const Info& info);
    ~PlayerController();

    virtual void update(float dt) override;
    void on_feet_collision(const physics::Manifold& manifold);
    void on_body_collision(const physics::Manifold& manifold);
    void mouse_move_callback(glm::vec2 mouse);
    void mouse_scroll_callback(input::Mouse::Wheel wheel, float delta);
    void respawn(glm::vec3 position);
    vpg::Listener mouse_move_listener;
    vpg::Listener mouse_scroll_listener;

    ecs::Entity entity;
    ecs::Entity camera;
    ecs::Entity torso;
    ecs::Entity lfoot, rfoot;
    ecs::Entity lhand, rhand;
    ecs::Entity feet_collider;

    float camera_distance, camera_x, camera_y;
    bool on_floor, respawned;

    float time;
    glm::vec3 torso_pos;
    glm::vec3 lfoot_pos, rfoot_pos;
    glm::vec3 lhand_pos, rhand_pos;

    glm::vec3 velocity;
    glm::vec3 floor_velocity, last_dir;

    glm::vec2 last_mouse;
    float sensitivity;
};
