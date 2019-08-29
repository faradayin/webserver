//
// Created by Feng on 2019/8/13.
//

#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H

#include "base/noncopyable.h"
#include "EventLoopThread.h";
#include "base/Logging.h"
#include <memory>
#include <vector>

//线程池
//使用一个vector保存EventLoopThread的指针列表: threads_
//使用一个vector保存EventLoop的指针列表: loops_
class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseloop, int numThreads);
    ~EventLoopThreadPool()
    {
        LOG<<"~EventLoopThreadPool()";
    }
    void start();
    EventLoop* getNextLoop();

private:
    EventLoop* baseloop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

#endif //WEBSERVER_EVENTLOOPTHREADPOOL_H
