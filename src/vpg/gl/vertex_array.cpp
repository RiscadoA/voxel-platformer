#include <vpg/gl/vertex_array.hpp>

#include <GL/glew.h>

#include <iostream>

using namespace vpg;
using namespace vpg::gl;

bool VertexArray::create(VertexArray& va, std::initializer_list<Attribute> attributes) {
    GLuint vao;
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (auto& attribute : attributes) {
        GLenum gl_type;
        GLboolean gl_normalized;

        switch (attribute.type) {
            case Attribute::Type::U8: {
                gl_type = GL_UNSIGNED_BYTE;
                gl_normalized = GL_FALSE;
                break;
            }

            case Attribute::Type::I8: {
                gl_type = GL_BYTE;
                gl_normalized = GL_FALSE;
                break;
            }

            case Attribute::Type::NU8: {
                gl_type = GL_UNSIGNED_BYTE;
                gl_normalized = GL_TRUE;
                break;
            }

            case Attribute::Type::NI8: {
                gl_type = GL_BYTE;
                gl_normalized = GL_TRUE;
                break;
            }

            case Attribute::Type::U32: {
                gl_type = GL_UNSIGNED_INT;
                gl_normalized = GL_FALSE;
                break;
            }

            case Attribute::Type::I32: {
                gl_type = GL_INT;
                gl_normalized = GL_FALSE;
                break;
            }

            case Attribute::Type::NU32: {
                gl_type = GL_UNSIGNED_INT;
                gl_normalized = GL_TRUE;
                break;
            }

            case Attribute::Type::NI32: {
                gl_type = GL_INT;
                gl_normalized = GL_TRUE;
                break;
            }

            case Attribute::Type::F32: {
                gl_type = GL_FLOAT;
                gl_normalized = GL_FALSE;
                break;
            }

            default: {
                std::abort(); // Unreachable code
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer.vbo);
        glVertexAttribPointer(
            attribute.shader_location,
            attribute.size,
            gl_type,
            gl_normalized,
            GLsizei(attribute.stride),
            (const void*)attribute.offset
        );

        glEnableVertexAttribArray(attribute.shader_location);
    }

    
    auto err = glGetError();
    if (err != 0) {
        std::cerr << "VertexArray::create() failed:\nglGetError() returned " << err << '\n';
        return false;
    }

    va.vao = vao;
    return true;
}

VertexArray::VertexArray(unsigned int vao) :
    vao(vao) {
    // Empty
}

VertexArray::VertexArray(VertexArray&& rhs) {
    if (this->vao != 0) {
        glDeleteVertexArrays(1, &this->vao);
    }

    this->vao = rhs.vao;
    rhs.vao = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& rhs) {
    if (this->vao != 0) {
        glDeleteVertexArrays(1, &this->vao);
    }

    this->vao = rhs.vao;
    rhs.vao = 0;

    return *this;
}

VertexArray::~VertexArray() {
    if (this->vao != 0) {
        glDeleteVertexArrays(1, &this->vao);
    }
}

void VertexArray::bind() const {
#ifndef NDEBUG
    if (this->vao == 0) {
        std::abort(); // This shouldn't happen
    }
#endif
    glBindVertexArray(this->vao);
}
