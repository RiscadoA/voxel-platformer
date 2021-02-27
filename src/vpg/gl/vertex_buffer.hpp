#pragma once

#include <string>

#include <vpg/gl/usage.hpp>

namespace vpg::gl {
    class VertexBuffer final {
    public:
        inline VertexBuffer() : vbo(0) {}
        VertexBuffer(VertexBuffer&& rhs);
        VertexBuffer& operator=(VertexBuffer&& rhs);
        ~VertexBuffer();

        static bool create(VertexBuffer& vb, size_t size, const void* data, Usage usage);

        void bind();
        bool update(size_t offset, size_t size, const void* data);
        void* map();
        void unmap();

    private:
        friend class VertexArray;

        VertexBuffer(unsigned int vbo);

        unsigned int vbo;
    };
}
