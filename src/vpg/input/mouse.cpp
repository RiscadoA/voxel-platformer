#include <vpg/input/mouse.hpp>
#include <vpg/input/window.hpp>

#include <GLFW/glfw3.h>

using namespace vpg;
using namespace vpg::input;

Event<Mouse::Button> Mouse::ButtonUp;
Event<Mouse::Button> Mouse::ButtonDown;
Event<Mouse::Wheel, float> Mouse::Scroll;
Event<glm::vec2> Mouse::Move;

static glm::vec2 mouse_pos;
static bool mouse_state[(size_t)Mouse::Button::Count] = {};

static Mouse::Button from_glfw_button(int button) {
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        return Mouse::Button::Left;
    case GLFW_MOUSE_BUTTON_RIGHT:
        return Mouse::Button::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        return Mouse::Button::Middle;
    case GLFW_MOUSE_BUTTON_4:
        return Mouse::Button::Extra1;
    case GLFW_MOUSE_BUTTON_5:
        return Mouse::Button::Extra2;
    default:
        return Mouse::Button::Invalid;
    }
}

static void glfw_mouse_button_callback(GLFWwindow* win, int button, int action, int mods) {
    auto b = from_glfw_button(button);
    if (b == Mouse::Button::Invalid) {
        return;
    }

    switch (action) {
    case GLFW_PRESS:
        mouse_state[(int)b] = true;
        Mouse::ButtonDown.fire(b);
        break;
    case GLFW_RELEASE:
        mouse_state[(int)b] = false;
        Mouse::ButtonUp.fire(b);
        break;
    }
}

static void glfw_cursor_position_callback(GLFWwindow* window, double x, double y) {
    mouse_pos.x = (float)x;
    mouse_pos.y = (float)y;
    Mouse::Move.fire(mouse_pos);
}

static void glfw_scroll_callback(GLFWwindow* window, double x, double y) {
    if (x != 0.0) {
        Mouse::Scroll.fire(Mouse::Wheel::Horizontal, x);
    }
    
    if (y != 0.0) {
        Mouse::Scroll.fire(Mouse::Wheel::Vertical, x);
    }
}

void Mouse::set_mode(Mode mode) {
    auto window = (GLFWwindow*)Window::get_handle();
    switch (mode) {
    case Mode::Normal:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case Mode::Disabled:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    }
}

Mouse::Mode Mouse::get_mode() {
    auto window = (GLFWwindow*)Window::get_handle();
    switch (glfwGetInputMode(window, GLFW_CURSOR)) {
    case GLFW_CURSOR_NORMAL:
        return Mode::Normal;
    case GLFW_CURSOR_DISABLED:
        return Mode::Disabled;
    default:
        abort();
    }
}

bool Mouse::is_button_pressed(Button button) {
    return mouse_state[(int)button];
}

glm::vec2 Mouse::get_position() {
    return mouse_pos;
}

void vpg::input::Mouse::set_position(const glm::vec2& position) {
    auto window = (GLFWwindow*)Window::get_handle();
    glfwSetCursorPos(window, (double)position.x, (double)position.y);
}

void Mouse::init() {
    glfwSetMouseButtonCallback((GLFWwindow*)Window::get_handle(), &glfw_mouse_button_callback);
    glfwSetCursorPosCallback((GLFWwindow*)Window::get_handle(), &glfw_cursor_position_callback);
    glfwSetScrollCallback((GLFWwindow*)Window::get_handle(), &glfw_scroll_callback);
}
