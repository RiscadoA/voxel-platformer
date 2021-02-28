#pragma once

#include <vpg/ecs/component.hpp>
#include <vpg/ecs/entity_manager.hpp>

#include <unordered_map>
#include <iostream>

namespace vpg::ecs {
	class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void entity_destroyed(Entity entity) = 0;
	};

	template <typename T>
	class ComponentArray : public IComponentArray {
	public:
		static_assert(std::is_base_of<Component, T>::value);

		ComponentArray();

		T& insert(Entity entity, T&& component);
		void remove(Entity entity);
		T* get(Entity entity);

		virtual void entity_destroyed(Entity entity) override;

	private:
		T components[MaxEntities];
		std::unordered_map<size_t, Entity> index_to_entity;
		std::unordered_map<Entity, size_t> entity_to_index;
		size_t count;
	};

	template<typename T>
	inline ComponentArray<T>::ComponentArray() {
		this->count = 0;
	}

	template<typename T>
	inline T& ComponentArray<T>::insert(Entity entity, T&& component) {
		if (this->entity_to_index.find(entity) != this->entity_to_index.end()) {
			std::cerr << "vpg::ecs::ComponentArray<T>::insert() failed:\n"
					  << "Entity " << entity << " already has this component\n";
			abort();
		}

		this->components[this->count].~T();
		new (&this->components[this->count]) T(std::move(component));
		this->index_to_entity.emplace(this->count, entity);
		this->entity_to_index.emplace(entity, this->count);
		return this->components[this->count++];
	}

	template<typename T>
	inline void ComponentArray<T>::remove(Entity entity) {
		auto it = this->entity_to_index.find(entity);
		if (it == this->entity_to_index.end()) {
			std::cerr << "vpg::ecs::ComponentArray<T>::remove() failed:\n"
					  << "Entity " << entity << " doesn't have this component\n";
			abort();
		}

		auto index = it->second;
		this->entity_to_index.erase(it);

		this->count -= 1;
		this->components[index].~T();
		new (&this->components[this->count]) T(std::move(this->components[this->count]));
		auto last_entity = this->index_to_entity[this->count];
		this->index_to_entity.erase(this->count);
		this->entity_to_index[last_entity] = index;
		this->index_to_entity[index] = last_entity;
	}

	template<typename T>
	inline T* ComponentArray<T>::get(Entity entity) {
		auto it = this->entity_to_index.find(entity);
		if (it == this->entity_to_index.end()) {
			return nullptr;
		}

		return &this->components[it->second];
	}

	template<typename T>
	inline void ComponentArray<T>::entity_destroyed(Entity entity) {
		auto it = this->entity_to_index.find(entity);
		if (it != this->entity_to_index.end()) {
			this->remove(entity);
		}
	}
}