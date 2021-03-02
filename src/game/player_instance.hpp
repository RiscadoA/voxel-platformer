#include <vpg/ecs/behaviour.hpp>

#include <vpg/data/text.hpp>

#include <vpg/input/mouse.hpp>
#include <vpg/input/keyboard.hpp>

using namespace vpg;

struct PlayerInstance : public ecs::IBehaviour {
    static constexpr char TypeName[] = "PlayerInstance";

    struct Info : public IBehaviour::Info {
        data::Handle<data::Text> scene;
        glm::vec3 position;

        virtual bool serialize(memory::Stream& stream) const override;
        virtual bool deserialize(memory::Stream& stream) override;
    };

    PlayerInstance(vpg::ecs::Entity entity, const Info& info);
    ~PlayerInstance();

    ecs::Entity player;
};
