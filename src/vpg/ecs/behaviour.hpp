#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <map>
#include <string>
#include <functional>
#include <iostream>

namespace vpg::ecs {
    class IBehaviour {
    public:
        virtual ~IBehaviour() = default;

        inline virtual void update(float dt) {}

        virtual void serialize(std::ostream& os) = 0;
        virtual void deserialize(std::istream& is) = 0;
    };

    class Behaviour : public Component {
    public:
        static constexpr char TypeName[] = "Behaviour";

        Behaviour();
        Behaviour(Behaviour&& rhs) noexcept;
        Behaviour(const Behaviour&) = delete;
        ~Behaviour();

        template<typename T, typename ... TArgs>
        static Behaviour create(Entity entity, TArgs... args);
        template<typename T>
        static void register_type();

        void update(float dt);

        virtual void serialize(std::ostream& os) override;
        virtual void deserialize(std::istream& is) override;

    private:
        IBehaviour* behaviour;
        std::string type_name;
        Entity entity;
        static std::map<std::string, std::function<IBehaviour*(Entity, std::istream& is)>> registry;
    };

    template<typename T, typename ...TArgs>
    inline static Behaviour Behaviour::create(Entity entity, TArgs ...args) {
        static_assert(std::is_base_of<IBehaviour, T>::value);
        Behaviour ret;
        ret.behaviour = new T(entity, args...);
        ret.type_name = T::TypeName;
        ret.entity = entity;
        return std::move(ret);
    }

    template<typename T>
    inline void Behaviour::register_type() {
        static_assert(std::is_base_of<IBehaviour, T>::value);
        
        if (registry.find(T::TypeName) != registry.end()) {
            std::cerr << "vpg::ecs::Behaviour::register_type() failed:\n"
                      << "Behaviour type already registered\n";
            abort();
        }

        registry.emplace(T::TypeName, [](Entity entity, std::istream& is) -> IBehaviour* {
            IBehaviour* ret;
            ret = new T(entity);
            ret->deserialize(is);
            return ret;
        });
    }

    class BehaviourSystem : public System {
    public:
        BehaviourSystem();

        void update(float dt);
    };
}