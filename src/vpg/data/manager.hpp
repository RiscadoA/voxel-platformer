#pragma once

#include <vpg/data/asset.hpp>
#include <vpg/data/handle.hpp>

#include <string>
#include <map>
#include <iostream>
#include <functional>

namespace vpg::data {
    class Manager {
    public:
        Manager() = delete;
        ~Manager() = delete;

        static bool init();
        static void terminate();

        // Must be called before Manager::init()
        static void register_loader(
            std::string type,
            std::function<void* (Asset*)> load_fn,
            std::function<void(Asset*)> unload_fn
        );

        template <typename T>
        static Handle<T> load(const std::string& id);

    private:
        struct Loader {
            std::function<void* (Asset*)> load_fn;
            std::function<void(Asset*)> unload_fn;
        };

        static std::map<std::string, Loader> loaders;
        static std::map<std::string, Asset*> assets;
    };

    template<>
    Handle<void> Manager::load<void>(const std::string& id);

    template<typename T>
    inline Handle<T> Manager::load(const std::string& id) {
        auto generic_handle = Manager::load<void>(id);
        if (generic_handle.get_asset() == nullptr) {
            return Handle<T>(nullptr);
        }

        if (generic_handle.get_asset()->get_type() != T::Type) {
            std::cerr << "vpg::data::Manager::load() failed:\n";
            std::cerr << "Couldn't find asset '" << id << "':\n";
            std::cerr << "Asset type mismatch: expected '" << T::Type << "', found '" << generic_handle.get_asset()->get_type() << "'\n";
            abort();
        }
        return Handle<T>(generic_handle.get_asset());
    }
}
