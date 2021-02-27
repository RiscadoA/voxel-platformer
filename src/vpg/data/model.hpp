#pragma once

#include <vpg/data/manager.hpp>

#include <vpg/gl/mesh.hpp>
#include <vpg/gl/palette.hpp>

namespace vpg::data {
    class Model {
    public:
        static inline constexpr char Type[] = "model";

        static void* load(Asset* asset);
        static void unload(Asset* asset);

    private:
        Model() = default;
        ~Model() = default;

        gl::Matrix matrix;
        gl::Mesh mesh;
        gl::Palette palette;
    };
}