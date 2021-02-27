#include <vpg/gl/palette.hpp>

#include <gl/glew.h>
#include <cstring>

using namespace vpg::gl;

Palette::Palette() {
    this->mats = nullptr;
    this->ubo = 0;
}

Palette::Palette(Palette&& rhs) {
    this->mats = rhs.mats;
    this->ubo = rhs.ubo;
    rhs.mats = nullptr;
    rhs.ubo = 0;
}

Palette& Palette::operator=(Palette&& rhs) {
    if (this->mats != nullptr) {
        delete[] this->mats;
    }

    if (this->ubo != 0) {
        glDeleteBuffers(1, &this->ubo);
    }

    this->mats = rhs.mats;
    this->ubo = rhs.ubo;
    rhs.mats = nullptr;
    rhs.ubo = 0;

    return *this;
}

Palette::~Palette() {
    if (this->mats != nullptr) {
        delete[] this->mats;
    }

    if (this->ubo != 0) {
        glDeleteBuffers(1, &this->ubo);
    }
}

bool Palette::create(Palette& palette, std::initializer_list<Material> mats) {
    palette.mats = new Material[255];
    memcpy(palette.mats, mats.begin(), mats.size() * sizeof(Material));
    
    glGenBuffers(1, &palette.ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, palette.ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Material) * 255, palette.mats, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return false;
}

void Palette::update() {
    glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material) * 255, this->mats);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void vpg::gl::Palette::bind(unsigned int index) {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, this->ubo);
}

Material& Palette::operator[](size_t index) {
    return this->mats[index];
}
