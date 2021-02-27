#pragma once

#include <vpg/data/manager.hpp>
#include <vpg/gl/shader.hpp>

namespace vpg::data {
    class Shader {
    public:
        static inline constexpr char Type[] = "shader";

        static void* load(Asset* asset);
        static void unload(Asset* asset);

        inline const gl::Shader& get_shader() const { return this->shader; }

    private:
        Shader() = default;
        ~Shader() = default;

        gl::Shader shader;
    };
}