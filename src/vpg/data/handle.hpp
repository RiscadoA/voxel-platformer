#pragma once

#include <vpg/data/asset.hpp>

namespace vpg::data {
    template<typename T>
    class Handle;

    template<>
    class Handle<void> {
    public:
        Handle(Asset* asset = nullptr);
        Handle(const Handle& rhs);
        Handle(Handle&& rhs) noexcept;
        Handle& operator=(Asset* asset);
        Handle& operator=(const Handle& rhs);
        Handle& operator=(Handle&& rhs) noexcept;
        ~Handle();

        Asset* get_asset();

    private:
        Asset* asset;
    };

    template<typename T>
    class Handle : public Handle<void> {
    public:
        using Handle<void>::Handle;

        inline T* operator->() { return (T*)this->get_asset()->get_data(); }
    };
}