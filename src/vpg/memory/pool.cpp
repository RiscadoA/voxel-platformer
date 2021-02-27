#include <vpg/memory/pool.hpp>

#include <cstring>

using namespace vpg::memory;

Pool<void>::Pool(size_t initial_count, size_t element_sz, std::align_val_t alignment) {
    this->used = 0;
    this->total = initial_count;
    this->element_sz = element_sz;
    this->alignment = alignment;

    if ((size_t)this->alignment > this->element_sz) {
        this->element_sz = (size_t)this->alignment;
    }
    else if (this->element_sz % (size_t)this->alignment != 0) {
        this->element_sz = (this->element_sz / (size_t)this->alignment + 1) * (size_t)this->alignment;
    }

    this->data = operator new(this->total * this->element_sz + this->total, this->alignment);
    this->state = (char*)this->data + this->total * this->element_sz;
    memset(this->state, 0, this->total);
}

Pool<void>::Pool(Pool&& rhs) {
    this->used = rhs.used;
    this->total = rhs.total;
    this->element_sz = rhs.element_sz;
    this->alignment = rhs.alignment;
    this->data = rhs.data;
    rhs.data = nullptr;
}

Pool<void>::~Pool() {
    if (this->data != nullptr) {
        operator delete(this->data, this->alignment);
    }
}

size_t Pool<void>::alloc() {
    if (this->used < this->total) {
        this->used += 1;

        for (size_t i = 0; i < this->total; ++i) {
            if (this->state[i] == 0) {
                this->state[i] = 1;
                return i;
            }
        }

        abort(); // Unreachable
    }
    else {
        this->used += 1;

        // Expand pool
        auto old_total = this->total;
        this->total *= 2;
        void* new_data = operator new(this->total * this->element_sz + this->total, this->alignment);
        this->state = (char*)this->data + this->total * this->element_sz;
        memset(this->state, 1, old_total + 1);
        memset(this->state + old_total + 1, 0, total - old_total - 1);
        memcpy(new_data, this->data, this->total * this->element_sz);
        operator delete(this->data, this->alignment);
        this->data = new_data;
        return old_total;
    }
}

void Pool<void>::free(size_t index) {
    this->state[index] = 0;
}

bool vpg::memory::Pool<void>::has_element(size_t index) {
    return index < this->total && this->state[index] != 0;
}
