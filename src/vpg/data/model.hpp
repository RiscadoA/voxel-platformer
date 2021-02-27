#pragma once

#include <vpg/data/manager.hpp>

#include <vpg/gl/matrix.hpp>
#include <vpg/gl/vertex_array.hpp>
#include <vpg/gl/vertex_buffer.hpp>
#include <vpg/gl/index_buffer.hpp>
#include <vpg/gl/palette.hpp>

namespace vpg::data {
    class Model {
    public:
        static inline constexpr char Type[] = "model";

        static void* load(Asset* asset);
        static void unload(Asset* asset);

        inline const gl::Matrix& get_matrix() const { return this->matrix; }
        inline const gl::Palette& get_palette() const { return this->palette; }
        inline const gl::VertexArray& get_vertex_array() const { return this->va; }
        inline const gl::IndexBuffer& get_index_buffer() const { return this->ib; }
        inline size_t get_index_count() const { return this->index_count; }

    private:
        Model() = default;
        ~Model() = default;

        gl::Matrix matrix;
        gl::Palette palette;
        gl::VertexArray va;
        gl::VertexBuffer vb;
        gl::IndexBuffer ib;
        size_t index_count;
    };
}