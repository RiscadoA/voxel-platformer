#pragma once

namespace vpg::memory {
    template <typename T>
    inline T swap_bytes(T u) {
        union {
            T u;
            unsigned char b[sizeof(T)];
        } src, dst;

        for (size_t i = 0; i < sizeof(T); ++i) {
            dst.b[i] = src.b[sizeof(T) - i - 1];
        }

        return dst.u;
    }

    inline bool is_big_endian() {
        union {
            int i;
            unsigned char c[sizeof(int)];
        } u;
        u.i = 1;
        return u.c[0];
    }

    template <typename T>
    inline T from_little_endian(T u) {
        if (is_big_endian()) {
            return swap_bytes(u);
        }
        else {
            return u;
        }
    }

    template <typename T>
    inline T to_little_endian(T u) {
        if (is_big_endian()) {
            return swap_bytes(u);
        }
        else {
            return u;
        }
    }

    template <typename T>
    inline T from_big_endian(T u) {
        if (!is_big_endian()) {
            return swap_bytes(u);
        }
        else {
            return u;
        }
    }

    template <typename T>
    inline T to_big_endian(T u) {
        if (!is_big_endian()) {
            return swap_bytes(u);
        }
        else {
            return u;
        }
    }
}