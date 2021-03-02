#include <vpg/ecs/behaviour.hpp>

#include <vpg/data/text.hpp>

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>

using namespace vpg;

struct PlayerController : public ecs::IBehaviour {
    static constexpr char TypeName[] = "PlayerController";

    struct Info : public IBehaviour::Info {
        ecs::Entity torso;
        ecs::Entity lfoot, rfoot;
        ecs::Entity lhand, rhand;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    PlayerController(ecs::Entity entity, const Info& info);
    ~PlayerController();

    ecs::Entity entity;
    ecs::Entity torso;
    ecs::Entity lfoot, rfoot;
    ecs::Entity lhand, rhand;
};
