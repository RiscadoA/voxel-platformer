#include <vpg/ecs/behaviour.hpp>

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>

struct PlayerController : public vpg::ecs::IBehaviour {
    static constexpr char TypeName[] = "PlayerController";

    struct Info : public vpg::ecs::IBehaviour::Info {
        virtual bool serialize(vpg::memory::Stream& stream) const override;
        virtual bool deserialize(vpg::memory::Stream& stream) override;
    };

    PlayerController(vpg::ecs::Entity entity, const Info& info);

    virtual void update(float dt) override;

    vpg::ecs::Entity entity;
    float velocity;
};
