#pragma once

#include <string>

namespace vpg::gl {
    class Shader final {
    public:
        static bool create(Shader& shader, const char* vs, const char* fs);
        
        inline Shader() : program(0), vs(0), fs(0) {}
        Shader(Shader&& rhs);
        Shader& operator=(Shader&& rhs);
        ~Shader();

        void bind() const;
        int get_attribute_location(const char* name) const;
        int get_uniform_location(const char* name) const;
        void bind_uniform_buffer(const char* name, unsigned int binding) const;

    private:
        Shader(unsigned int program, unsigned int vs, unsigned int fs);

        unsigned int program, vs, fs;
    };
}