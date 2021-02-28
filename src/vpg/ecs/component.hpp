#pragma once

#include <iostream>

namespace vpg::ecs {
    class Component {
    public:
        Component() = default;
        Component(const Component&) = delete;
        Component(Component&&) = default;
        virtual ~Component() = default;

        virtual void serialize(std::ostream& os) = 0;
        virtual void deserialize(std::istream& is) = 0;
    };
}