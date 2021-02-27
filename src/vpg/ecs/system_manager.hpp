#pragma once

#include <vpg/ecs/entity_manager.hpp>

#include <set>
#include <unordered_map>
#include <iostream>

namespace vpg::ecs {
	class SystemManager;

	class System {
	public:
		virtual ~System() = default;

	public:
		friend SystemManager;

		Signature signature;
		std::set<Entity> entities;
	};

	class SystemManager {
	public:
		template<typename T, typename ... TArgs>
		T* register_system(TArgs... args);

		void entity_destroyed(Entity entity);
		void entity_signature_changed(Entity entity, Signature entity_signature);

	private:
		std::unordered_map<const char*, System*> systems;
	};

	template<typename T, typename ... TArgs>
	inline T* SystemManager::register_system(TArgs... args) {
		const char* type_name = typeid(T).name();

		if (this->systems.find(type_name) != this->systems.end()) {
			std::cerr << "vpg::ecs::SystemManager::register_system() failed:\n" 
					  << "System already registered\n";
			abort();
		}

		auto system = new T(args...);
		this->systems.emplace(type_name, system);
		return system;
	}
}