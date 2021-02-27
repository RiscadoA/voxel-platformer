#pragma once

#include <vpg/ecs/coordinator.hpp>
#include <vpg/data/model.hpp>

namespace vpg::gl {
    struct Renderable {
        enum class Type {
            None,
            Model,
        } type;

        union {
            data::Handle<data::Model> model;
        };

        Renderable();
        Renderable(data::Handle<data::Model> model);
        Renderable(Renderable&& rhs) noexcept;
        ~Renderable();
    };

    class RenderableSystem : public ecs::System {
    public:
        RenderableSystem();
    };
}