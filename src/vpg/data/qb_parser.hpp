#pragma once

#include <vpg/gl/mesh.hpp>
#include <vpg/gl/palette.hpp>

#include <vector>
#include <fstream>

namespace vpg::data {
    bool parse_qb(gl::Matrix& matrix, gl::Palette& palette, std::ifstream& ifs);
}