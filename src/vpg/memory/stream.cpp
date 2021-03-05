#include <vpg/memory/stream.hpp>

#include <iostream>

using namespace vpg::memory;

Stream::Stream() {
    this->failed_flag = false;
}

void Stream::write_ref(int64_t ref) {
    auto it = this->ref_map.top().find(ref);
    if (it == this->ref_map.top().end()) {
        auto new_ref = (int64_t)this->ref_map.top().size();
        this->ref_map.top()[ref] = new_ref;
        this->write_i64(new_ref);
    }
    else {
        this->write_i64(it->second);
    }
}

int64_t Stream::read_ref() {
    int64_t ref = this->read_ref_custom();
    if (ref == -1) {
        return -1;
    }

    for (auto& p : this->ref_map.top()) {
        if (p.second == ref) {
            return p.first;
        }
    }

    std::cerr << "vpg::memory::Stream::read_ref(): failed\n"
              << "Reference '" << ref << "' read but no mappings found\n";
    this->set_failed();
    return -1;
}

void vpg::memory::Stream::add_ref_map(int64_t write, int64_t read) {
    this->ref_map.top()[write] = read;
}

int64_t vpg::memory::Stream::ref_read_to_write(int64_t ref) {
    for (auto& p : this->ref_map.top()) {
        if (p.second == ref) {
            return p.first;
        }
    }

    std::cerr << "vpg::memory::Stream::ref_read_to_write(): failed\n"
              << "Reference '" << ref << "' read but no mappings found\n";
    return -1;
}

int64_t vpg::memory::Stream::ref_write_to_read(int64_t ref) {
    auto it = this->ref_map.top().find(ref);
    if (it == this->ref_map.top().end()) {
        auto new_ref = (int64_t)this->ref_map.top().size();
        this->ref_map.top()[ref] = new_ref;
    	return new_ref;
    }
    else {
        return it->second;
    }
}

void Stream::push_ref_map() {
    this->ref_map.emplace();
    this->push_ref_map_custom();
}

void Stream::pop_ref_map() {
    this->pop_ref_map_custom();
    this->ref_map.pop();
}

void Stream::set_failed() {
    this->failed_flag = true;
}

bool Stream::failed() const {
    return this->failed_flag;
}
