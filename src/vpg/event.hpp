#pragma once

#include <functional>
#include <unordered_map>

namespace vpg {
    using Listener = uint32_t;

    template <typename ... TArgs>
    class Event {
    public:
        Event() = default;
        Event(Event&&) = default;

        Listener add_listener(std::function<void(TArgs...)> listener);
        void remove_listener(Listener listener);
        void fire(TArgs... args);

    private:
        std::unordered_map<Listener, std::function<void(TArgs...)>> listeners = {};
        Listener next_listener = 0;
    };

    template<typename ... TArgs>
    inline Listener Event<TArgs...>::add_listener(std::function<void(TArgs...)> listener) {
        this->listeners.emplace(this->next_listener, listener);
        return this->next_listener++;
    }

    template<typename ... TArgs>
    inline void Event<TArgs...>::remove_listener(Listener listener) {
        this->listeners.erase(listener);
    }

    template<typename ... TArgs>
    inline void Event<TArgs...>::fire(TArgs ... args) {
        for (auto& l : this->listeners) {
            l.second(args...);
        }
    }
}