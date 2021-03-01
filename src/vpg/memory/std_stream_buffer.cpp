#include <vpg/memory/std_stream_buffer.hpp>

using namespace vpg::memory;

STDStreamBuffer::STDStreamBuffer(FILE* file) {
    this->file = file;
}

size_t STDStreamBuffer::write(const void* data, size_t size) {
    return fwrite(data, 1, size, this->file);
}

size_t STDStreamBuffer::read(void* data, size_t size) {
    return fread(data, 1, size, this->file);
}
