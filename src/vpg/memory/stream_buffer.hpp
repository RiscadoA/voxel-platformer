#pragma once

#include <cstddef>

namespace vpg::memory {
    class StreamBuffer {
    public:
        StreamBuffer() = default;
        StreamBuffer(StreamBuffer&&) = default;
        StreamBuffer(const StreamBuffer&) = delete;
        virtual ~StreamBuffer() = default;

        virtual size_t write(const void* data, size_t size) = 0;
        virtual size_t read(void* data, size_t size) = 0;
    };
}
