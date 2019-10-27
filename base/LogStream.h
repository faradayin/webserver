//
// Created by Feng on 2019/8/30.
//

#ifndef WEBSERVER_LOGSTREAM_H
#define WEBSERVER_LOGSTREAM_H

#include "noncopyable.h"
#include <assert.h>
#include <string.h>
#include <string>
// LogStream主要用来格式化输出，重载了<<运算符，
// 同时也有自己的一块缓冲区，这里缓冲区的存在是为了缓存一行，
// 把多个<<的结果连成一块
class AsyncLogging;
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

//封装一个char数组
template <int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer()
    :   cur_(data_)
    {}

    ~FixedBuffer()
    {}

    void append(const char* buf, size_t len)
    {
        if(avail() > static_cast<int> len)
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const
    {
        return data();
    }

    int length() const { return static_cast<int>(cur_ - data_); }

    char* current() { return cur_; }

    //可用空间
    int avail() const { return static_cast<int>(end() - cur_); }

    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }

    //置零
    void bzero() { memset(data_, 0, sizeof data_); }

private:
    char data_[SIZE];
    char* cur_;

    const char* end() const
    {
        return data_ + sizeof data_;
    }
};

class LogStream : noncopyable
{
private:
    Buffer buffer_;
    static const int kMaxNumericSize = 32;

    template<typename T>
    void formatInteger(T);

    void staticCheck();

public:
    typedef FixedBuffer<kSmallBuffer> Buffer;
    LogStream& operator<<(bool v)
    {
        buffer_.append(v? "1" : "0", 1);
        return *this;
    }

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(const void*);

    LogStream& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double);
    LogStream& operator<<(long double);

    LogStream& operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }

    LogStream& operator<<(const char* str)
    {
        if (str)
            buffer_.append(str, strlen(str));
        else
            buffer_.append("(null)", 6);
        return *this;
    }

    LogStream& operator<<(const unsigned char* str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

    void append(const char* data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }
};


#endif //WEBSERVER_LOGSTREAM_H
