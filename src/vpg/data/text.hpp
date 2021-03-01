#pragma once

#include <vpg/data/manager.hpp>

#include <string>

namespace vpg::data {
    class Text {
    public:
        static inline constexpr char Type[] = "text";

        static void* load(Asset* asset);
        static void unload(Asset* asset);

        inline const std::string& get_content() const { return this->content; }

    private:
        Text() = default;
        ~Text() = default;

        std::string content;
    };
}