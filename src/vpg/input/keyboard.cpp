#include <vpg/input/keyboard.hpp>
#include <vpg/input/window.hpp>

#include <GLFW/glfw3.h>

using namespace vpg;
using namespace vpg::input;

Event<Keyboard::Key> Keyboard::Down;
Event<Keyboard::Key> Keyboard::Up;

static bool keyboard_state[(size_t)Keyboard::Key::Count] = {};

static Keyboard::Key from_glfw_key(int key) {
	switch (key) {
#define ADDKEY(glfw, key) case GLFW_KEY_##glfw: return Keyboard::Key::key; break
		ADDKEY(A, A);
		ADDKEY(B, B);
		ADDKEY(C, C);
		ADDKEY(D, D);
		ADDKEY(E, E);
		ADDKEY(F, F);
		ADDKEY(G, G);
		ADDKEY(H, H);
		ADDKEY(I, I);
		ADDKEY(J, J);
		ADDKEY(K, K);
		ADDKEY(L, L);
		ADDKEY(M, M);
		ADDKEY(N, N);
		ADDKEY(O, O);
		ADDKEY(P, P);
		ADDKEY(Q, Q);
		ADDKEY(R, R);
		ADDKEY(S, S);
		ADDKEY(T, T);
		ADDKEY(U, U);
		ADDKEY(V, V);
		ADDKEY(W, W);
		ADDKEY(X, X);
		ADDKEY(Y, Y);
		ADDKEY(Z, Z);
		ADDKEY(0, Num0);
		ADDKEY(1, Num1);
		ADDKEY(2, Num2);
		ADDKEY(3, Num3);
		ADDKEY(4, Num4);
		ADDKEY(5, Num5);
		ADDKEY(6, Num6);
		ADDKEY(7, Num7);
		ADDKEY(8, Num8);
		ADDKEY(9, Num9);
		ADDKEY(ESCAPE, Escape);
		ADDKEY(LEFT_CONTROL, LControl);
		ADDKEY(LEFT_SHIFT, LShift);
		ADDKEY(LEFT_ALT, LAlt);
		ADDKEY(LEFT_SUPER, LSystem);
		ADDKEY(RIGHT_CONTROL, RControl);
		ADDKEY(RIGHT_SHIFT, RShift);
		ADDKEY(RIGHT_ALT, RAlt);
		ADDKEY(RIGHT_SUPER, RSystem);
		ADDKEY(MENU, Menu);
		ADDKEY(LEFT_BRACKET, LBracket);
		ADDKEY(RIGHT_BRACKET, RBracket);
		ADDKEY(SEMICOLON, SemiColon);
		ADDKEY(COMMA, Comma);
		ADDKEY(PERIOD, Period);
		ADDKEY(APOSTROPHE, Quote);
		ADDKEY(SLASH, Slash);
		ADDKEY(BACKSLASH, BackSlash);
		ADDKEY(GRAVE_ACCENT, Tilde);
		ADDKEY(EQUAL, Equal);
		ADDKEY(MINUS, Subtract);
		ADDKEY(SPACE, Space);
		ADDKEY(ENTER, Return);
		ADDKEY(BACKSPACE, BackSpace);
		ADDKEY(TAB, Tab);
		ADDKEY(PAGE_UP, PageUp);
		ADDKEY(PAGE_DOWN, PageDown);
		ADDKEY(END, End);
		ADDKEY(HOME, Home);
		ADDKEY(INSERT, Insert);
		ADDKEY(DELETE, Delete);
		ADDKEY(KP_ADD, Add);
		ADDKEY(KP_SUBTRACT, Subtract);
		ADDKEY(KP_MULTIPLY, Multiply);
		ADDKEY(KP_DIVIDE, Divide);
		ADDKEY(LEFT, Left);
		ADDKEY(RIGHT, Right);
		ADDKEY(UP, Up);
		ADDKEY(DOWN, Down);
		ADDKEY(KP_0, Numpad0);
		ADDKEY(KP_1, Numpad1);
		ADDKEY(KP_2, Numpad2);
		ADDKEY(KP_3, Numpad3);
		ADDKEY(KP_4, Numpad4);
		ADDKEY(KP_5, Numpad5);
		ADDKEY(KP_6, Numpad6);
		ADDKEY(KP_7, Numpad7);
		ADDKEY(KP_8, Numpad8);
		ADDKEY(KP_9, Numpad9);
		ADDKEY(F1, F1);
		ADDKEY(F2, F2);
		ADDKEY(F3, F3);
		ADDKEY(F4, F4);
		ADDKEY(F5, F5);
		ADDKEY(F6, F6);
		ADDKEY(F7, F7);
		ADDKEY(F8, F8);
		ADDKEY(F9, F9);
		ADDKEY(F10, F10);
		ADDKEY(F11, F11);
		ADDKEY(F12, F12);
		ADDKEY(PAUSE, Pause);
#undef ADDKEY
		default:
			return Keyboard::Key::Invalid;
	}
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto k = from_glfw_key(key);
	if (k == Keyboard::Key::Invalid) {
		return;
	}

	switch (action) {
	case GLFW_PRESS:
		keyboard_state[(int)k] = true;
		Keyboard::Down.fire(k);
		break;
	case GLFW_RELEASE:
		keyboard_state[(int)k] = false;
		Keyboard::Up.fire(k);
		break;
	}
}

bool vpg::input::Keyboard::is_key_pressed(Key key) {
    return keyboard_state[(int)key];
}

void vpg::input::Keyboard::init() {
	auto window = (GLFWwindow*)Window::get_handle();
	glfwSetKeyCallback(window, &glfw_key_callback);
}
