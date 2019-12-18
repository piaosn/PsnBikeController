// #include <arduino.h>
#include "pb_arduino.h"
#include <Print.h>
#include <Stream.h>

static bool pb_print_write(pb_ostream_t *stream, const pb_byte_t *buf, size_t count) {
    // Serial.println(count);
    Print* p = reinterpret_cast<Print*>(stream->state);
    return p->write(buf, count) == count;
};

pb_ostream_s as_pb_ostream(Print* p) {
    // 串口最大缓存为64bytes, 等做实验后确定这里要不要做修改
    return {pb_print_write, p, SIZE_MAX, 0};
};

static bool pb_stream_read(pb_istream_t *stream, pb_byte_t *buf, size_t count) {
    Stream* s = reinterpret_cast<Stream*>(stream->state);
    return s->readBytes(buf, count) == count;
};

pb_istream_s as_pb_istream(Stream* s) {
    // 串口最大缓存为64bytes, 等做实验后确定这里要不要做修改
    return {pb_stream_read, s, SIZE_MAX, 0};
};
