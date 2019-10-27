//
// Created by Feng on 2019/8/30.
//

#ifndef WEBSERVER_ASYNCLOGGING_H
#define WEBSERVER_ASYNCLOGGING_H

#include "CountDownLatch.h"
#include "MutexLock.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <functional>
#include <string>
#include <vector>
// AsyncLogging是核心，它负责启动一个log线程，专门用来将log写入LogFile，应用了“双缓冲技术”，其实有4个以上的缓冲区，但思想是一样的。
// AsyncLogging负责(定时到或被填满时)将缓冲区中的数据写入LogFile中
// AsyncLogging是如何启动一个线程的
class AsyncLogging : noncopyable
{
private:

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;

    bool running_;
    const int flushInterval_;
    std::string basename_;

    Thread thread_;
    MutexLock mutex_;
    Condition cond_;

    BufferVector buffers_;//核心结构，保存所有buffer的指针
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;

    CountDownLatch latch_;

    void threadFunc();

public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging()
    {
        if(running_) stop();
    }


    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void append(const char* logline, int len);

};


#endif //WEBSERVER_ASYNCLOGGING_H
