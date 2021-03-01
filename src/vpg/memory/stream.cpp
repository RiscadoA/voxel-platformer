#include <vpg/memory/stream.hpp>

using namespace vpg::memory;

Stream::Stream() {
    this->failed_flag = false;
}

void Stream::set_failed() {
    this->failed_flag = true;
}

bool Stream::failed() const {
    return this->failed_flag;
}
