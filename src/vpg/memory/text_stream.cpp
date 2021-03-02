#include <vpg/memory/text_stream.hpp>
#include <iostream>

using namespace vpg::memory;

TextStream::TextStream(StreamBuffer* buf) {
    this->buf = buf;
    this->new_line = false;
}

void TextStream::write_u8(uint8_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_u16(uint16_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_u32(uint32_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_u64(uint64_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_i8(int8_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_i16(int16_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_i32(int32_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_i64(int64_t val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_f32(float val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_f64(double val) {
    std::string str = std::to_string(val);
    str = str + ' ';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = true;
}

void TextStream::write_string(const std::string& in_str) {
    std::string str = in_str;
    if (str.find('\n') != std::string::npos || (str.size() > 0 && (str[0] == '#' || str[0] == '"' || str[0] == '$'))) {
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\"') {
                str = str.substr(0, i) + "\\\"" + str.substr(i + 1);
            }
            else if (str[i] == '\\') {
                str = str.substr(0, i) + "\\\\" + str.substr(i + 1);
                i += 1;
            }
        }
        str = "\"" + str + "\"";
    }
 
    if (this->new_line) {
        str = '\n' + str;
    }
    str = str + '\n';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = false;
}

void TextStream::write_comment(const std::string& comment, int header) {
    std::string str = "#";
    for (int i = 0; i < header; ++i) {
        str += '#';
    }
    if (this->new_line) {
        str = '\n' + str;
    }
    str = str + ' ' + comment + '\n';
    if (this->buf->write(str.data(), str.size()) != str.size()) {
        this->set_failed();
    }
    this->new_line = false;
}

uint8_t TextStream::read_u8() {
    uint64_t x = this->read_u64();
    if (x > UINT8_MAX) {
        std::cerr << "vpg::memory::TextStream::read_u8() failed:\n"
                  << "Integer overflow";
        this->set_failed();
    }
    return (uint8_t)x;
}

uint16_t TextStream::read_u16() {
    uint64_t x = this->read_u64();
    if (x > UINT16_MAX) {
        std::cerr << "vpg::memory::TextStream::read_u16() failed:\n"
                  << "Integer overflow";
        this->set_failed();
    }
    return (uint16_t)x;
}

uint32_t TextStream::read_u32() {
    uint64_t x = this->read_u64();
    if (x > UINT32_MAX) {
        std::cerr << "vpg::memory::TextStream::read_u32() failed:\n"
                  << "Integer overflow";
        this->set_failed();
    }
    return (uint32_t)x;
}

uint64_t TextStream::read_u64() {
    std::string str;

    // Skip whitespace
    auto c = this->get_char();
    while (c == '\n' || c == ' ' || c == '\t' || c == '#') {
        if (c == '#') {
            while (c != '\n') {
                c = this->get_char();
            }
        }
        c = this->get_char();
    }

    // Get number
    while (c != '\n' && c != ' ' && c != '\t' && c != '\0') {
        str += c;
        c = this->get_char();
    }

    try {
        return std::stoull(str);
    }
    catch (std::invalid_argument) {
        std::cerr << "vpg::memory::TextStream::read_u64() failed:\n"
                  << "Couldn't parse unsigned integer (std::invalid_argument thrown by std::stoull)\n";
        this->set_failed();
        return 0;
    }
}

int8_t TextStream::read_i8() {
    int64_t x = this->read_i64();
    if (x < INT8_MIN || x > INT8_MAX) {
        std::cerr << "vpg::memory::TextStream::read_i8() failed:\n"
                  << "Integer overflow";
        this->set_failed();
    }
    return (int8_t)x;
}

int16_t TextStream::read_i16() {
    int64_t x = this->read_i64();
    if (x < INT16_MIN || x > INT16_MAX) {
        std::cerr << "vpg::memory::TextStream::read_i16() failed:\n"
                  << "Integer overflow";
        this->set_failed();
    }
    return (int16_t)x;
}

int32_t TextStream::read_i32() {
    int64_t x = this->read_i64();
    if (x < INT32_MIN || x > INT32_MAX) {
        std::cerr << "vpg::memory::TextStream::read_i32() failed:\n"
            << "Integer overflow";
        this->set_failed();
    }
    return (int32_t)x;
}

int64_t TextStream::read_i64() {
    std::string str;

    // Skip whitespace
    auto c = this->get_char();
    while (c == '\n' || c == ' ' || c == '\t' || c == '#') {
        if (c == '#') {
            while (c != '\n') {
                c = this->get_char();
            }
        }
        c = this->get_char();
    }

    // Get number
    while (c != '\n' && c != ' ' && c != '\t' && c != '\0') {
        str += c;
        c = this->get_char();
    }

    try {
        return std::stoll(str);
    }
    catch (std::invalid_argument) {
        std::cerr << "vpg::memory::TextStream::read_i64() failed:\n"
                  << "Couldn't parse integer (std::invalid_argument thrown by std::stoll)\n";
        this->set_failed();
        return 0;
    }
}

float TextStream::read_f32() {
    return (float)this->read_f64();
}

double TextStream::read_f64() {
    std::string str;

    // Skip whitespace
    auto c = this->get_char();
    while (c == '\n' || c == ' ' || c == '\t' || c == '#') {
        if (c == '#') {
            while (c != '\n') {
                c = this->get_char();
            }
        }
        c = this->get_char();
    }

    // Get number
    while (c != '\n' && c != ' ' && c != '\t' && c != '\0') {
        str += c;
        c = this->get_char();
    }

    try {
        return std::stod(str);
    }
    catch (std::invalid_argument) {
        std::cerr << "vpg::memory::TextStream::read_f64() failed:\n"
                  << "Couldn't parse floating point (std::invalid_argument thrown by std::stod)\n";
        this->set_failed();
        return 0;
    }
}

std::string TextStream::read_string() {
    std::string str;

    // Skip whitespace
    auto c = this->get_char();
    while (c == '\n' || c == ' ' || c == '\t' || c == '#') {
        if (c == '#') {
            while (c != '\n') {
                c = this->get_char();
            }
        }
        c = this->get_char();
    }

    if (c == '\"') {
        c = this->get_char();

        while (c != '\"' && c != '\0') {
            if (c == '\\') {
                c = this->get_char();
            }
            
            str += c;

            c = this->get_char();
        }
    }
    else {
        while (c != '\n' && c != '\0') {
            str += c;
            c = this->get_char();
        }
    }

    return str;
}

void TextStream::clear_ref_map_custom() {
    this->str_to_index.clear();
}

int64_t TextStream::read_ref_custom() {
    // Skip whitespace
    auto c = this->get_char();
    while (c == '\n' || c == ' ' || c == '\t' || c == '#') {
        if (c == '#') {
            while (c != '\n') {
                c = this->get_char();
            }
        }
        c = this->get_char();
    }

    if (c == '$') {
        std::string str;

        // Get identifier
        c = this->get_char();
        while (c != ' ' && c != '\t' && c != '\n' && c != '\0') {
            str += c;
            c = this->get_char();
        }

        if (str == "Null") {
            return -1;
        }

        auto it = this->str_to_index.find(str);
        if (it == this->str_to_index.end()) {
            this->str_to_index[str] = (int64_t)this->str_to_index.size();
            return (int64_t)this->str_to_index.size() - 1;
        }
        else {
            return it->second;
        }
    }
    else {
        std::string str;

        // Get number
        while (c != '\n' && c != ' ' && c != '\t' && c != '\0') {
            str += c;
            c = this->get_char();
        }

        try {
            return std::stoll(str);
        }
        catch (std::invalid_argument) {
            std::cerr << "vpg::memory::TextStream::read_ref_custom() failed:\n"
                      << "Couldn't parse integer (std::invalid_argument thrown by std::stoll)\n";
            this->set_failed();
            return 0;
        }
    }
}

char vpg::memory::TextStream::get_char() {
    char c;
    if (this->buf->read(&c, sizeof(c)) == 0) {
        return '\0';
    }
    return c;
}
