#pragma once

#include <new>

namespace vpg::memory {
    template<typename T>
    class Pool;

    template<>
    class Pool<void> {
    public:
        Pool(size_t initial_count, size_t element_sz, std::align_val_t alignment);
        Pool(Pool&& rhs);
        Pool(const Pool&) = delete;
        ~Pool();

        size_t alloc();
        void free(size_t index);

        bool has_element(size_t index);

    protected:
        void* data;
        char* state;
        size_t used, total, element_sz;
        std::align_val_t alignment;
    };

    template <typename T>
    class Pool : public Pool<void> {
    public:
        Pool(size_t initial_count);

        T* operator[](size_t index);
    };

    template<typename T>
    inline Pool<T>::Pool(size_t initial_count) : Pool<void>(initial_count, sizeof(T), alignof(T)) {}

    template<typename T>
    inline T* Pool<T>::operator[](size_t index) {
        return (T*)((char*)this->data + index * this->element_sz);
    }
}