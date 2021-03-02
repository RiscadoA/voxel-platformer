#pragma once

#include <vpg/ecs/coordinator.hpp>

namespace vpg::ecs {
    class Scene : public System {
    public:
        Scene();
        ~Scene() = default;
        
        void clean();

        bool deserialize(memory::Stream& stream);
        static Entity deserialize_tree(memory::Stream& stream);
    };
}