#pragma once

#include <vpg/ecs/coordinator.hpp>

#include <map>
#include <string>
#include <functional>
#include <iostream>

namespace vpg::ecs {
    class IBehaviour {
    public:
        IBehaviour() = default;
        virtual ~IBehaviour() = default;

        class Info {
        public:
            Info() = default;
            Info(Info&&) = default;
            virtual ~Info() = default;

            virtual bool serialize(memory::Stream& stream) const = 0;
            virtual bool deserialize(memory::Stream& stream) = 0;
        };

        inline virtual void update(float dt) {}
    };

    class Behaviour {
    public:
        static constexpr char TypeName[] = "Behaviour";

        class Info {
        public:
            Info();
            Info(Info&& rhs) noexcept;
            ~Info();

            template<typename T>
            static Info create(typename T::Info&& info);

            bool serialize(memory::Stream& stream) const;
            bool deserialize(memory::Stream& stream);

        private:
            friend Behaviour;

            std::string name;
            IBehaviour::Info* info;
        };

        Behaviour(Entity entity, const Info& info);
        Behaviour(Behaviour&& rhs) noexcept;
        Behaviour(const Behaviour&) = delete;
        ~Behaviour();

        template<typename T>
        static void register_type();

        void update(float dt);

    private:
        std::string name;
        IBehaviour* behaviour;

        static std::map<std::string, std::function<IBehaviour::Info*()>> info_constructors;
        static std::map<std::string, std::function<IBehaviour*(Entity, const IBehaviour::Info*)>> constructors;
    };

    template<typename T>
    inline void Behaviour::register_type() {
        static_assert(std::is_base_of<IBehaviour, T>::value);
        static_assert(std::is_base_of<IBehaviour::Info, T::Info>::value);
        
        if (Behaviour::constructors.find(T::TypeName) != Behaviour::constructors.end()) {
            std::cerr << "vpg::ecs::Behaviour::register_type() failed:\n"
                      << "Behaviour type already registered\n";
            abort();
        }

        Behaviour::info_constructors.emplace(T::TypeName, []() -> IBehaviour::Info* {
            return new T::Info();
        });

        Behaviour::constructors.emplace(T::TypeName, [](Entity entity, const IBehaviour::Info* create_info) -> IBehaviour* {
            return new T(entity, *((const T::Info*)create_info));
        });
    }

    class BehaviourSystem : public System {
    public:
        BehaviourSystem();

        void update(float dt);
    };

    template<typename T>
    inline static Behaviour::Info Behaviour::Info::create(typename T::Info&& info) {
        Info ret;
        ret.info = new typename T::Info(std::move(info));
        ret.name = T::TypeName;
        return std::move(ret);
    }
}