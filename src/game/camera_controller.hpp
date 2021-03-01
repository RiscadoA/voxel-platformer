#include <vpg/ecs/behaviour.hpp>

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>

class CameraController : public vpg::ecs::IBehaviour {
public:
    static constexpr char TypeName[] = "CameraController";

    struct Info : public vpg::ecs::IBehaviour::Info {
        virtual bool serialize(vpg::memory::Stream& stream) const override;
        virtual bool deserialize(vpg::memory::Stream& stream) override;
    };

    CameraController(vpg::ecs::Entity entity, const Info& info);
    ~CameraController();

    virtual void update(float dt) override;

private:
    void mouse_move_callback(glm::vec2 mouse);
    vpg::Listener mouse_move_listener;

    vpg::ecs::Entity entity;
    glm::vec2 last_mouse;
    float sensitivity, speed;
};
