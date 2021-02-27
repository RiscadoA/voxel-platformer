#include <vpg/data/asset.hpp>

#include <iostream>

using namespace vpg::data;

Asset::Asset(
    const std::string& id,
    const std::string& type,
    const std::string& args,
    bool is_dynamic,
    std::function<void* (Asset*)> load_fn,
    std::function<void(Asset*)> unload_fn
) {
    this->id = id;
    this->type = type;
    this->args = args;
    this->is_dynamic = is_dynamic;
    this->load_fn = load_fn;
    this->unload_fn = unload_fn;
    this->data = nullptr;
}

Asset::~Asset() {
    if (this->data != nullptr) {
        if (this->ref_count != 0) {
            std::cerr << "vpg::data::Asset::~Asset() called even though there are still references to this asset\n";
        }
        this->unload_fn(this);
    }
}

void Asset::inc_ref() {
    if (++this->ref_count == 1 && this->data == nullptr) {
        this->data = this->load_fn(this);
        if (this->data == nullptr) {
            std::cerr << "vpg::data::Asset::inc_ref() failed:\n";
            std::cerr << "Load function returned null\n";
            abort();
        }
    }
}

void Asset::dec_ref() {
    if (--this->ref_count == 0 && this->is_dynamic) {
        this->unload_fn(this);
    }
}
