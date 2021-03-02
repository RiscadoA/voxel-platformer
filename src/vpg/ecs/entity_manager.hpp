#pragma once

#include <cstdint>
#include <bitset>
#include <queue>

namespace vpg::ecs {
    using ComponentType = uint8_t;
    constexpr ComponentType MaxComponents = 32;

    using Entity = int32_t;
    using Signature = std::bitset<MaxComponents>;
    constexpr Entity MaxEntities = 8192;
    constexpr Entity NullEntity = -1;

    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;

        Entity create();
        void destroy(Entity entity);

        void set_signature(Entity entity, Signature signature);
        Signature get_signature(Entity entity);

    private:
        std::queue<Entity> available;
        Signature signatures[MaxEntities];
        size_t count;
    };
}