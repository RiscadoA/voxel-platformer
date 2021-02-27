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

        inline const gl::Matrix& get_matrix() const { return this->matrix; }
        inline const gl::Mesh& get_mesh() const { return this->mesh; }
        inline const gl::Palette& get_palette() const { return this->palette; }

    private:
        Model() = default;
        ~Model() = default;

        gl::Matrix matrix;
        gl::Mesh mesh;
        gl::Palette palette;
    };
}