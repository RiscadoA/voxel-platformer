#pragma once

#include <vpg/ecs/entity_manager.hpp>

#include <unordered_map>
#include <iostream>
#include <new>

namespace vpg::ecs {
	class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void entity_destroyed(Entity entity) = 0;
	};

	template <typename T>
	class ComponentArray : public IComponentArray {
	public:
		ComponentArray();
		virtual ~ComponentArray() override;

		T& insert(Entity entity, const typename T::Info& create_info);
		void remove(Entity entity);
		T* get(Entity entity);
		Entity get_entity(T& component);

		virtual void entity_destroyed(Entity entity) override;

	private:
		T* components;
		std::unordered_map<size_t, Entity> index_to_entity;
		std::unordered_map<Entity, size_t> entity_to_index;
		size_t count;
	};

	template<typename T>
	inline ComponentArray<T>::ComponentArray() {
		this->count = 0;
		this->components = (T*)operator new(sizeof(T) * (size_t)MaxEntities, (std::align_val_t)alignof(T));
	}

	template<typename T>
	inline ComponentArray<T>::~ComponentArray() {
		for (int i = 0; i < this->count; ++i) {
			this->components[i].~T();
		}
		operator delete((void*)this->components, (std::align_val_t)alignof(T));
	}

	template<typename T>
	inline T& ComponentArray<T>::insert(Entity entity, const typename T::Info& create_info) {
		if (this->entity_to_index.find(entity) != this->entity_to_index.end()) {
			std::cerr << "vpg::ecs::ComponentArray<T>::insert() failed:\n"
					  << "Entity " << entity << " already has this component\n";
			abort();
		}

		size_t index = this->count++;
		new (&this->components[index]) T(entity, create_info);
		this->index_to_entity.emplace(index, entity);
		this->entity_to_index.emplace(entity, index);
		return this->components[index];
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
		size_t last = this->count;
		this->components[index].~T();
		if (last != index) {
			new (&this->components[index]) T(std::move(this->components[last]));
			this->components[last].~T();
			auto last_entity = this->index_to_entity[last];
			this->index_to_entity.erase(last);
			this->entity_to_index[last_entity] = index;
			this->index_to_entity[index] = last_entity;
		}
		else {
			this->index_to_entity.erase(index);
		}
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
	inline Entity ComponentArray<T>::get_entity(T& component) {
		size_t index = &component - &this->components[0];
		return this->index_to_entity[index];
	}

	template<typename T>
	inline void ComponentArray<T>::entity_destroyed(Entity entity) {
		auto it = this->entity_to_index.find(entity);
		if (it != this->entity_to_index.end()) {
			this->remove(entity);
		}
	}
}