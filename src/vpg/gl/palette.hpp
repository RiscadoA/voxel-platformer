#pragma once

#include <glm/glm.hpp>

namespace vpg::gl {
    // struct Material {
    //     vec3 color;
    //     float _padding;
    // };
    //
    // layout (std140) uniform Palette {
    //     Material mats[];
    // }
    //

    struct Material {
        glm::vec3 color;
        float _padding;
    };
    
    class Palette {
    public:
        Palette();
        Palette(Palette&& rhs);
        Palette& operator=(Palette&& rhs);
        ~Palette();

        static bool create(Palette& palette, std::initializer_list<Material> mats);
        void update();
        void bind(unsigned int index);

        Material& operator[](size_t index);

    private:
        Material* mats;
        unsigned int ubo;
    };
}