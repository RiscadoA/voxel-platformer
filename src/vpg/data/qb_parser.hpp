#pragma once

#include <vpg/gl/matrix.hpp>
#include <vpg/gl/palette.hpp>

#include <vector>
#include <fstream>

namespace vpg::data {
    bool parse_qb(gl::Matrix& matrix, gl::Palette& palette, std::ifstream& ifs, bool emissive);
}