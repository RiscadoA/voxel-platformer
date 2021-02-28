#pragma once

#include <vpg/ecs/entity_manager.hpp>
#include <vpg/ecs/component_array.hpp>

#include <iostream>
#include <unordered_map>

namespace vpg::ecs {
	class ComponentManager {
	public:
		ComponentManager();
		~ComponentManager();

		template<typename T>
		void register_component();

		template <typename T>
		ComponentType get_component_type();

		template <typename T>
		T& add_component(Entity entity, T&& component);
		
		template <typename T>
		void remove_component(Entity entity);

		template <typename T>
		T* get_component(Entity entity);

		void entity_destroyed(Entity entity);

	private:
		std::unordered_map<const char*, ComponentType> types;
		std::unordered_map<const char*, IComponentArray*> arrays;

		ComponentType next_type;
	};

	template<typename T>
	inline void ComponentManager::register_component() {
		const char* type_name = T::TypeName;

		if (this->types.find(type_name) != this->types.end()) {
			std::cerr << "vpg::ecs::ComponentManager::register_component() failed:\n"
				      << "Component type already registered\n";
			abort();
		}

		this->types.emplace(type_name, this->next_type);
		this->arrays.emplace(type_name, new ComponentArray<T>());

		this->next_type += 1;
	}

	template<typename T>
	inline ComponentType ComponentManager::get_component_type() {
		const char* type_name = T::TypeName;
		
		auto it = this->types.find(type_name);
		if (it == this->types.end()) {
			std::cerr << "vpg::ecs::ComponentManager::get_component_type() failed:\n"
					   << "Component type not registered\n";
			abort();
		}

		return it->second;
	}

	template<typename T>
	inline T& ComponentManager::add_component(Entity entity, T&& component) {
		const char* type_name = T::TypeName;

		auto it = this->arrays.find(type_name);
		if (it == this->arrays.end()) {
			std::cerr << "vpg::ecs::ComponentManager::add_component() failed:\n"
					  << "Component type not registered\n";
			abort();
		}

		return ((ComponentArray<T>*)it->second)->insert(entity, std::move(component));
	}

	template<typename T>
	inline void ComponentManager::remove_component(Entity entity) {
		const char* type_name = T::TypeName;

		auto it = this->arrays.find(type_name);
		if (it == this->arrays.end()) {
			std::cerr << "vpg::ecs::ComponentManager::remove_component() failed:\n"
					  << "Component type not registered\n";
			abort();
		}

		((ComponentArray<T>*)it->second)->remove(entity);
	}

	template<typename T>
	inline T* ComponentManager::get_component(Entity entity) {
		const char* type_name = T::TypeName;

		auto it = this->arrays.find(type_name);
		if (it == this->arrays.end()) {
			std::cerr << "vpg::ecs::ComponentManager::get_component() failed:\n"
					  << "Component type not registered\n";
			abort();
		}

		return ((ComponentArray<T>*)it->second)->get(entity);
	}
}