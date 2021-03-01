#pragma once

#include <vpg/memory/stream_buffer.hpp>

#include <string>

namespace vpg::memory {
    class StringStreamBuffer : public StreamBuffer {
    public:
        StringStreamBuffer(const std::string& str);

        virtual size_t write(const void* data, size_t size) override;
        virtual size_t read(void* data, size_t size) override;

    private:
        std::string str;
        size_t head;
    };
}