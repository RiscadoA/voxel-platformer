#pragma once

#include <string>
#include <cstdint>

namespace vpg::memory {
    class Stream {
    public:
        Stream();
        virtual ~Stream() = default;

        virtual void write_u8(uint8_t val) = 0;
        virtual void write_u16(uint16_t val) = 0;
        virtual void write_u32(uint32_t val) = 0;
        virtual void write_u64(uint64_t val) = 0;
        virtual void write_i8(int8_t val) = 0;
        virtual void write_i16(int16_t val) = 0;
        virtual void write_i32(int32_t val) = 0;
        virtual void write_i64(int64_t val) = 0;
        virtual void write_f32(float val) = 0;
        virtual void write_f64(double val) = 0;
        virtual void write_string(const std::string& str) = 0;
        virtual void write_comment(const std::string& comment, int header) = 0;

        virtual uint8_t read_u8() = 0;
        virtual uint16_t read_u16() = 0;
        virtual uint32_t read_u32() = 0;
        virtual uint64_t read_u64() = 0;
        virtual int8_t read_i8() = 0;
        virtual int16_t read_i16() = 0;
        virtual int32_t read_i32() = 0;
        virtual int64_t read_i64() = 0;
        virtual float read_f32() = 0;
        virtual double read_f64() = 0;
        virtual std::string read_string() = 0;

        void set_failed();
        bool failed() const;

    private:
        bool failed_flag;
    };
}