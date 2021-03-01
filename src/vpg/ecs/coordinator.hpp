#pragma once

#include <vpg/ecs/entity_manager.hpp>
#include <vpg/ecs/component_manager.hpp>
#include <vpg/ecs/system_manager.hpp>
#include <vpg/memory/stream.hpp>

#include <iostream>

namespace vpg::ecs {
    class Coordinator {
    public:
        Coordinator() = delete;
        ~Coordinator() = delete;

        static void init();
        static void terminate();

        static Entity create_entity();
        static void destroy_entity(Entity entity);

        template<typename T>
        static void register_component();
        
        template<typename T>
        static T& add_component(Entity entity, const typename T::Info& create_info);
        static bool add_component(Entity entity, memory::Stream& stream);

        template<typename T>
        static void remove_component(Entity entity);

        template<typename T>
        static T* get_component(Entity entity);

        template<typename T>
        static Entity get_component_entity(T& component);

        template<typename T>
        static ComponentType get_component_type();

        template<typename T, typename ... TArgs>
        static T* register_system(TArgs ... args);

    private:
        static EntityManager* entity_manager;
        static ComponentManager* component_manager;
        static SystemManager* system_manager;
    };

    template<typename T>
    inline void Coordinator::register_component() {
        Coordinator::component_manager->register_component<T>();
    }

    template<typename T>
    inline static T& Coordinator::add_component(Entity entity, const typename T::Info& create_info) {
        auto& ret = Coordinator::component_manager->add_component<T>(entity, create_info);
        auto signature = Coordinator::entity_manager->get_signature(entity);
        signature.set(Coordinator::component_manager->get_component_type<T>(), true);
        Coordinator::entity_manager->set_signature(entity, signature);
        Coordinator::system_manager->entity_signature_changed(entity, signature);
        return ret;
    }

    template<typename T>
    inline Entity Coordinator::get_component_entity(T& component) {
        return Coordinator::component_manager->get_component_entity<T>(component);
    }
    
    template<typename T>
    inline void Coordinator::remove_component(Entity entity) {
        Coordinator::component_manager->remove_component<T>(entity);
        auto signature = Coordinator::entity_manager->get_signature(entity);
        signature.set(Coordinator::component_manager->get_component_type<T>(), false);
        Coordinator::entity_manager->set_signature(entity, signature);
        Coordinator::system_manager->entity_signature_changed(entity, signature);
    }
    
    template<typename T>
    inline T* Coordinator::get_component(Entity entity) {
        return Coordinator::component_manager->get_component<T>(entity);
    }
    
    template<typename T>
    inline ComponentType Coordinator::get_component_type() {
        return Coordinator::component_manager->get_component_type<T>();
    }
    
    template<typename T, typename ... TArgs>
    inline T* Coordinator::register_system(TArgs ... args) {
        return Coordinator::system_manager->register_system<T>(args...);
    }
}