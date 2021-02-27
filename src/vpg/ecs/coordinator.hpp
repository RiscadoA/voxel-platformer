#pragma once

#include <vpg/ecs/entity_manager.hpp>
#include <vpg/ecs/component_manager.hpp>
#include <vpg/ecs/system_manager.hpp>

namespace vpg::ecs {
    class Coordinator {
    public:
        Coordinator() = default;
        Coordinator(const Coordinator&) = delete;

        Entity create_entity();
        void destroy_entity(Entity entity);

        template<typename T>
        void register_component();
        
        template<typename T>
        void add_component(Entity entity, T&& component);

        template<typename T>
        void remove_component(Entity entity);

        template<typename T>
        T* get_component(Entity entity);

        template<typename T>
        ComponentType get_component_type();

        template<typename T, typename ... TArgs>
        T* register_system(TArgs ... args);

    private:
        EntityManager entity_manager;
        ComponentManager component_manager;
        SystemManager system_manager;
    };

    template<typename T>
    inline void Coordinator::register_component() {
        this->component_manager.register_component<T>();
    }

    template<typename T>
    inline void Coordinator::add_component(Entity entity, T&& component) {
        this->component_manager.add_component<T>(entity, std::move(component));
        auto signature = this->entity_manager.get_signature(entity);
        signature.set(this->component_manager.get_component_type<T>(), true);
        this->entity_manager.set_signature(entity, signature);
        this->system_manager.entity_signature_changed(entity, signature);
    }
    
    template<typename T>
    inline void Coordinator::remove_component(Entity entity) {
        this->component_manager.remove_component<T>(entity);
        auto signature = this->entity_manager.get_signature(entity);
        signature.set(this->component_manager.get_component_type<T>(), false);
        this->entity_manager.set_signature(entity, signature);
        this->system_manager.entity_signature_changed(entity, signature);
    }
    
    template<typename T>
    inline T* Coordinator::get_component(Entity entity) {
        return this->component_manager.get_component<T>(entity);
    }
    
    template<typename T>
    inline ComponentType Coordinator::get_component_type() {
        return this->component_manager.get_component_type<T>();
    }
    
    template<typename T, typename ... TArgs>
    inline T* Coordinator::register_system(TArgs ... args) {
        return this->system_manager.register_system<T>(args...);
    }
}