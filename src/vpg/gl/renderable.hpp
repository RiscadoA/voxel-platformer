#pragma once

#include <vpg/ecs/coordinator.hpp>
#include <vpg/data/model.hpp>

namespace vpg::gl {
    struct Renderable : public ecs::Component {
        static constexpr char TypeName[] = "Renderable";

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

        virtual void serialize(std::ostream& os) override;
        virtual void deserialize(std::istream& is) override;
    };

    class RenderableSystem : public ecs::System {
    public:
        RenderableSystem();
    };
}