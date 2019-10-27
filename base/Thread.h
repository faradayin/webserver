//
// Created by Feng on 2019/8/18.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H

#include "CountDownLatch.h"
#include "noncopyable.h"
#include <functional>
#include <pthread.h>
#include <memory>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
//线程的创建和等待结束
class Thread : noncopyable
{
private:
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

    void setDefaultName();

public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const std::string &name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const {return started_;}
    pid_t tid() const {return tid_;}
    const std::string& name() const {return name_;}
};

#endif //WEBSERVER_THREAD_H
