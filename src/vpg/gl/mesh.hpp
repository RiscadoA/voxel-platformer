#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <vpg/gl/vertex_array.hpp>
#include <vpg/gl/vertex_buffer.hpp>
#include <vpg/gl/index_buffer.hpp>

namespace vpg::gl {
    struct Matrix {
        glm::ivec3 sz;
        std::vector<unsigned char> voxels;
    };

    struct Vertex {
        glm::u8vec3 pos;
        glm::i8vec3 normal;
        unsigned char material;
    };

    class Mesh final {
    public:
        Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&& rhs);
        ~Mesh() = default;

        void draw() const;

        bool update(const Matrix& matrix, bool generate_borders = true, bool gen_va = true);

        void generate_va();

    private:
        gl::VertexArray va;
        gl::VertexBuffer vb;
        gl::IndexBuffer ib;

        bool va_ready = false;
        int count;
    };
}