#pragma once

#include <atomic>
#include <string>
#include <functional>

namespace vpg::data {
    class Asset {
    public:
        Asset(
            const std::string& id,
            const std::string& type,
            const std::string& args,
            bool is_dynamic,
            std::function<void* (Asset*)> load_fn,
            std::function<void(Asset*)> unload_fn
        );

        ~Asset();

        inline const std::string& get_type() const { return this->type; }
        inline const std::string& get_args() const { return this->args; }
        inline void* get_data() const { return this->data; }

        void inc_ref();
        void dec_ref();

    private:
        std::string id, type, args;
        std::function<void*(Asset*)> load_fn;
        std::function<void(Asset*)> unload_fn;
        void* data;
        std::atomic<int> ref_count;
        bool is_dynamic;
    };
}