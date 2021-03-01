#include <vpg/memory/string_stream_buffer.hpp>

using namespace vpg::memory;

StringStreamBuffer::StringStreamBuffer(const std::string& str) {
    this->str = str;
    this->head = 0;
}

size_t StringStreamBuffer::write(const void* data, size_t size) {
    for (int i = 0; i < size; ++i) {
        this->str += ((const char*)data)[i];
    }
    return size;
}

size_t StringStreamBuffer::read(void* data, size_t size) {
    size_t read = 0;
    for (; read < size && this->head < this->str.size(); ++this->head, ++read) {
        ((char*)data)[read] = this->str[this->head];
    }
    return read;
}
