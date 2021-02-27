#pragma once

#include <vpg/gl/vertex_array.hpp>

#include <vector>
#include <glm/glm.hpp>

namespace vpg::gl {
    struct Matrix {
        glm::ivec3 sz;
        std::vector<unsigned char> voxels;
    };

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        unsigned char material;
    };

    bool matrix_to_mesh(
        std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        const Matrix& matrix,
        bool generate_borders = true
    );
}