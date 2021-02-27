#include <vpg/gl/index_buffer.hpp>

#include <iostream>
#include <GL/glew.h>

using namespace vpg;
using namespace vpg::gl;

bool IndexBuffer::create(IndexBuffer& ib, size_t size, const void* data, Usage usage) {
    GLuint ibo;
    GLenum gl_usage;

    if (usage == Usage::Static) {
        gl_usage = GL_STATIC_DRAW;
    } else if (usage == Usage::Dynamic) {
        gl_usage = GL_DYNAMIC_DRAW;
    } else if (usage == Usage::Stream) {
        gl_usage = GL_STREAM_DRAW;
    } else {
        abort(); // Unreachable code
    }

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, gl_usage);
    
    auto err = glGetError();
    if (err != 0) {
        std::cerr << "IndexBuffer::create() failed:\nglGetError() returned " << err;
        return false;
    }

    ib.ibo = ibo;
    return true;
}

IndexBuffer::IndexBuffer(unsigned int ibo) :
    ibo(ibo) {
    // Empty
}

IndexBuffer::IndexBuffer(IndexBuffer&& rhs) {
    if (this->ibo != 0) {
        glDeleteBuffers(1, &this->ibo);
    }

    this->ibo = rhs.ibo;
    rhs.ibo = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& rhs) {
    if (this->ibo != 0) {
        glDeleteBuffers(1, &this->ibo);
    }

    this->ibo = rhs.ibo;
    rhs.ibo = 0;
    
    return *this;
}

IndexBuffer::~IndexBuffer() {
    if (this->ibo != 0) {
        glDeleteBuffers(1, &this->ibo);
    }
}

void* IndexBuffer::map() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    auto ret = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (ret == nullptr) {
        std::cerr << "IndexBuffer::map() failed:\nglMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY) returned nullptr:\n"
                  << "glGetError() returned '" << glGetError() << "'\n";
    }

    return ret;
}

void IndexBuffer::unmap() {
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

bool IndexBuffer::update(size_t offset, size_t size, const void* data) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GLintptr(offset), GLsizeiptr(size), data);

    auto err = glGetError();
    if (err != 0) {
        std::cerr << "IndexBuffer::update() failed:\n"
                  << "glGetError() returned '" << err << "'\n";
        return false;
    }

    return true;
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
}
