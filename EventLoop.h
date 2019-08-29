//
// Created by Feng on 2019/8/5.
//

// One loop per thread意味着每个线程只能有一个EventLoop对象，EventLoop即是时间循环，每次从poller里拿活跃事件，并给到Channel里分发处理。
// EventLoop中的loop函数会在最底层(Thread)中被真正调用，开始无限的循环，直到某一轮的检查到退出状态后从底层一层一层的退出。

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include "base/Thread.h"
#include "Epoll.h"
#include "base/Logging.h"
#include "Channel.h"
#include "base/CurrentThread.h"
#include "Util.h"
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
using namespace std;
// you have time
class EventLoop
{
private:
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;

    //eventfd描述符当主线程把新连接分配给了某个SubReactor，该线程此时可能正阻塞在多路选择器(epoll)的等待中，
    // 怎么得知新连接的到来呢？
    // 这里使用了eventfd进行异步唤醒，线程会从epoll_wait中醒来，得到活跃事件，进行处理
    int wakeupFd_;
    const pid_t threadId_;

    std::vector<Functor> pendingFunctors_;//PendingFunctors的列表

    shared_ptr<Epoll> poller_;

    mutable MutexLock mutex_;//互斥量，本质上是一把锁

    shared_ptr<Channel> pwakeupChannel_;


    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}
    void assertInLoopThread()
    {
        assert(isInLoopThread());

    }
    void shutdown(shared_ptr<Channel> channel)
    {
        shutDownWR(channel->getFd());//这个函数哪里来的？？
    }
    void removeFromPoller(shared_ptr<Channel> channel)
    {
        poller_->epoll_del(channel);//poller_ 成员，智能指针管理的Epoll对象
    }
    void updatePoller(shared_ptr<Channel> channel, int timeout = 0)
    {
        poller_->epoll_mod(channel, timeout);
    }
    void addToPoller(shared_ptr<Channel> channel, int timeout = 0)
    {
        poller_->epoll_add(channel, timeout);
    }



};

#endif //WEBSERVER_EVENTLOOP_H
