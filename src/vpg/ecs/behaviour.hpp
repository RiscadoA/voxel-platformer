#pragma once

#include <vpg/ecs/coordinator.hpp>

namespace vpg::ecs {
    class IBehaviour {
    public:
        virtual ~IBehaviour() = default;

        inline virtual void update(float dt) {}
    };

    class Behaviour {
    public:
        Behaviour();
        template<typename T, typename ... TArgs>
        static Behaviour create(Entity entity, TArgs... args);
        Behaviour(Behaviour&& rhs) noexcept;
        Behaviour(const Behaviour&) = delete;
        ~Behaviour();

        void update(float dt);

    private:
        IBehaviour* behaviour;
    };

    template<typename T, typename ...TArgs>
    inline static Behaviour Behaviour::create(Entity entity, TArgs ...args) {
        static_assert(std::is_base_of<IBehaviour, T>::value);
        Behaviour ret;
        ret.behaviour = new T(entity, args...);
        return std::move(ret);
    }

    class BehaviourSystem : public System {
    public:
        BehaviourSystem();

        void update(float dt);
    };
}