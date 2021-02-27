#include <vpg/gl/vertex_buffer.hpp>

#include <GL/glew.h>

#include <iostream>

using namespace vpg;
using namespace vpg::gl;

bool VertexBuffer::create(VertexBuffer& vb, size_t size, const void* data, Usage usage) {
    GLuint vbo;
    GLenum gl_usage;

    if (usage == Usage::Static) {
        gl_usage = GL_STATIC_DRAW;
    } else if (usage == Usage::Dynamic) {
        gl_usage = GL_DYNAMIC_DRAW;
    } else if (usage == Usage::Stream) {
        gl_usage = GL_STREAM_DRAW;
    } else {
        std::abort(); // Unreachable code
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, gl_usage);
    
    auto err = glGetError();
    if (err != 0) {
        std::cerr << "VertexBuffer::create() failed:\nglGetError() returned " << err << '\n';
        return false;
    }

    vb.vbo = vbo;
    return true;
}

VertexBuffer::VertexBuffer(unsigned int vbo) :
    vbo(vbo) {
    // Empty
}

VertexBuffer::VertexBuffer(VertexBuffer&& rhs) {
    if (this->vbo != 0) {
        glDeleteBuffers(1, &this->vbo);
    }

    this->vbo = rhs.vbo;
    rhs.vbo = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs) {
    if (this->vbo != 0) {
        glDeleteBuffers(1, &this->vbo);
    }

    this->vbo = rhs.vbo;
    rhs.vbo = 0;
    
    return *this;
}

VertexBuffer::~VertexBuffer() {
    if (this->vbo != 0) {
        glDeleteBuffers(1, &this->vbo);
    }
}

void* VertexBuffer::map() {
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void VertexBuffer::unmap() {
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
}

bool VertexBuffer::update(size_t offset, size_t size, const void* data) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, GLintptr(offset), GLsizeiptr(size), data);

    auto err = glGetError();
    if (err != 0) {
        std::cerr << "VertexBuffer::update() failed:\n"
                  << "glGetError() returned '" << err << "'";
        return false;
    }

    return true;
}
