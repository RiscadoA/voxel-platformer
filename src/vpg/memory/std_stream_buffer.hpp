#pragma once

#include <vpg/memory/stream_buffer.hpp>

#include <cstdio>
#include <cstddef>

namespace vpg::memory {
    class STDStreamBuffer : public StreamBuffer {
    public:
        STDStreamBuffer(FILE* file);

        virtual size_t write(const void* data, size_t size) override;
        virtual size_t read(void* data, size_t size) override;

    private:
        FILE* file;
    };
}
