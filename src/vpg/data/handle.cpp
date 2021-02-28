#include <vpg/data/handle.hpp>
#include <vpg/data/manager.hpp>

using namespace vpg::data;

Handle<void>::Handle(Asset* asset) {
    this->asset = asset;
    if (this->asset != nullptr) {
        this->asset->inc_ref();
    }
}

Handle<void>::Handle(const Handle& rhs) {
    this->asset = rhs.asset;
    if (this->asset != nullptr) {
        this->asset->inc_ref();
    }
}

Handle<void>::Handle(Handle&& rhs) noexcept {
    this->asset = rhs.asset;
    rhs.asset = nullptr;
}

Handle<void>& Handle<void>::operator=(Asset* asset) {
    if (this->asset != nullptr) {
        this->asset->dec_ref();
        this->asset = nullptr;
    }

    this->asset = asset;
    if (this->asset != nullptr) {
        this->asset->inc_ref();
    }

    return *this;
}

Handle<void>& Handle<void>::operator=(const Handle& rhs) {
    if (this->asset != nullptr) {
        this->asset->dec_ref();
        this->asset = nullptr;
    }

    this->asset = rhs.asset;
    if (this->asset != nullptr) {
        this->asset->inc_ref();
    }

    return *this;
}

Handle<void>& Handle<void>::operator=(Handle&& rhs) noexcept {
    if (this->asset != nullptr) {
        this->asset->dec_ref();
        this->asset = nullptr;
    }

    this->asset = rhs.asset;
    rhs.asset = nullptr;

    return *this;
}

Handle<void>::~Handle() {
    if (this->asset != nullptr) {
        this->asset->dec_ref();
        this->asset = nullptr;
    }
}

Asset* Handle<void>::get_asset() {
    return this->asset;
}