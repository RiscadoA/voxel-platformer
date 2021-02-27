#include <vpg/gl/shader.hpp>

#include <GL/glew.h>

#include <iostream>

using namespace vpg;
using namespace vpg::gl;

bool Shader::create(Shader& shader, const char* vs_src, const char* fs_src) {
    GLuint program, vs, fs;
    GLint status;

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, nullptr);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (!status) {
        std::string info_log;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &status);
        info_log.resize(status);
        glGetShaderInfoLog(vs, GLsizei(info_log.size()) + 1, nullptr, &info_log[0]);
        info_log = "Shader::create() failed:\nglCreateShader(GL_VERTEX_SHADER) failed:\n" + info_log;
        std::cerr << info_log << '\n';
        return false;
    }

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (!status) {
        std::string info_log;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &status);
        info_log.resize(status);
        glGetShaderInfoLog(fs, GLsizei(info_log.size()) + 1, nullptr, &info_log[0]);
        info_log = "Shader::create() failed:\nglCreateShader(GL_FRAGMENT_SHADER) failed:\n" + info_log;
        std::cerr << info_log << '\n';
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        std::string info_log;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status);
        info_log.resize(status);
        glGetProgramInfoLog(program, GLsizei(info_log.size()) + 1, nullptr, &info_log[0]);
        info_log = "Shader::create() failed:\nglLinkProgram() failed:\n" + info_log;
        std::cerr << info_log << '\n';
        return false;
    }

    shader.program = program;
    shader.vs = vs;
    shader.fs = fs;
    return true;
}

Shader::Shader(unsigned int program, unsigned int vs, unsigned int fs) :
    program(program), vs(vs), fs(fs) {
    // Empty
}

Shader::Shader(Shader&& rhs) {
    this->program = rhs.program;
    this->vs = rhs.vs;
    this->fs = rhs.fs;
    rhs.program = 0;
    rhs.vs = 0;
    rhs.fs = 0;
}

Shader& Shader::operator=(Shader&& rhs) {
    if (this->program != 0) {
        glDeleteProgram(this->program);
    }

    if (this->vs != 0) {
        glDeleteShader(this->vs);
    }

    if (this->fs != 0) {
        glDeleteShader(this->fs);
    }

    this->program = rhs.program;
    this->vs = rhs.vs;
    this->fs = rhs.fs;
    rhs.program = 0;
    rhs.vs = 0;
    rhs.fs = 0;
    
    return *this;
}

Shader::~Shader() {
    if (this->program != 0) {
        glDeleteProgram(this->program);
    }

    if (this->vs != 0) {
        glDeleteShader(this->vs);
    }

    if (this->fs != 0) {
        glDeleteShader(this->fs);
    }
}

void Shader::bind() const {
    glUseProgram(this->program);
}

int Shader::get_attribute_location(const char* name) const {
    auto location = glGetAttribLocation(this->program, name);
    if (location < 0) {
        std::cerr << "Shader::get_attribute_location() failed:\nNo attribute '" << name << "' found";
    }
    return location;
}

int Shader::get_uniform_location(const char* name) const {
    auto location = glGetUniformLocation(this->program, name);
    if (location < 0) {
        std::cerr << "Shader::get_uniform_location() failed:\nNo uniform '" << name << "' found";
    }
    return location;
}