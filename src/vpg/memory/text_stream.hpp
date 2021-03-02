#pragma once

#include <vpg/memory/stream.hpp>
#include <vpg/memory/stream_buffer.hpp>

#include <map>
#include <stack>

namespace vpg::memory {
    class TextStream : public Stream {
    public:
        TextStream(StreamBuffer* buf);
        
        virtual void write_u8(uint8_t val) override;
        virtual void write_u16(uint16_t val) override;
        virtual void write_u32(uint32_t val) override;
        virtual void write_u64(uint64_t val) override;
        virtual void write_i8(int8_t val) override;
        virtual void write_i16(int16_t val) override;
        virtual void write_i32(int32_t val) override;
        virtual void write_i64(int64_t val) override;
        virtual void write_f32(float val) override;
        virtual void write_f64(double val) override;
        virtual void write_string(const std::string& str) override;
        virtual void write_comment(const std::string& comment, int header) override;
        virtual uint8_t read_u8() override;
        virtual uint16_t read_u16() override;
        virtual uint32_t read_u32() override;
        virtual uint64_t read_u64() override;
        virtual int8_t read_i8() override;
        virtual int16_t read_i16() override;
        virtual int32_t read_i32() override;
        virtual int64_t read_i64() override;
        virtual float read_f32() override;
        virtual double read_f64() override;
        virtual std::string read_string() override;

    protected:
        virtual void push_ref_map_custom() override;
        virtual void pop_ref_map_custom() override;
        virtual int64_t read_ref_custom() override;

    private:
        char get_char();

        std::stack<std::map<std::string, int64_t>> str_to_index;

        StreamBuffer* buf;
        bool new_line;
    };
}