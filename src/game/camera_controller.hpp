#include <vpg/ecs/behaviour.hpp>

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
    vpg::ecs::Entity entity;
    float sensitivity, speed;
};
