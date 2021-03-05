#pragma once

#include <vpg/gl/usage.hpp>

#include <cstdint>
#include <cstddef>

namespace vpg::gl {
    class IndexBuffer final {
    public:
        inline IndexBuffer() : ibo(0) {}
        IndexBuffer(IndexBuffer&& rhs);
        IndexBuffer& operator=(IndexBuffer&& rhs);
        ~IndexBuffer();

        static bool create(IndexBuffer& ib, size_t size, const void* data, Usage usage);

        bool update(size_t offset, size_t size, const void* data);
        void* map();
        void unmap();
        void bind() const;

    private:
        IndexBuffer(unsigned int ibo);

        unsigned int ibo;
    };
}
