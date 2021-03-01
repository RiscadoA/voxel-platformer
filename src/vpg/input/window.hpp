#pragma once

#include <vpg/event.hpp>

#include <glm/glm.hpp>

namespace vpg::input {
    class Window {
    public:
        static Event<glm::ivec2> FramebufferResized;
        
        static bool init();
        static void terminate();

        static void* get_handle();
        static glm::ivec2 get_framebuffer_size();
        static bool should_close();
        static void poll_events();
        static void swap_buffers();

    private:
        static void* handle;
    };
}