#pragma once

#include <vpg/ecs/coordinator.hpp>
#include <vpg/data/model.hpp>
#include <vpg/memory/stream.hpp>

namespace vpg::gl {
    struct Renderable {
        static constexpr char TypeName[] = "Renderable";

        enum class Type {
            None,
            Model,
        } type;

        struct Info {
            Type type;
            data::Handle<data::Model> model;

            bool serialize(memory::Stream& stream) const;
            bool deserialize(memory::Stream& stream);
        };

        Renderable(ecs::Entity entity, const Info& create_info);
        Renderable(Renderable&& rhs) noexcept;
        ~Renderable();

        union {
            data::Handle<data::Model> model;
        };
    };

    class RenderableSystem : public ecs::System {
    public:
        RenderableSystem();
    };
}