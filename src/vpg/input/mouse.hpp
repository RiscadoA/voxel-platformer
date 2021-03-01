#pragma once

#include <vpg/event.hpp>

#include <glm/glm.hpp>

namespace vpg::input {
    class Mouse {
    public:
        enum class Button {
            Invalid = -1,

            Left,
            Right,
            Middle,
            Extra1,
            Extra2,

            Count
        };

        enum class Wheel {
            Invalid = -1,

            Vertical,
            Horizontal,

            Count
        };

        enum class Mode {
            Normal,
            Disabled,
        };

        static Event<Button> ButtonUp;
        static Event<Button> ButtonDown;
        static Event<Wheel, float> Scroll;
        static Event<glm::vec2> Move;

        static void set_mode(Mode mode);
        static Mode get_mode();

        static bool is_button_pressed(Button button);
        static glm::vec2 get_position();
        static void set_position(const glm::vec2& position);

    private:
        friend class Window;

        static void init();
    };
}