//
// Created by Feng on 2019/8/5.
//

#include "EventLoop.h"
#include "base/Logging.h"
#include "Util.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <iostream>

using namespace std;

__thread EventLoop* t_loopInThisThread = 0; //线程

int createEventfd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
    {
        LOG<< "Failed  in eventfd";
        abort();
    }
    return evtfd;
}

//初始化EventLoop对象：
// 1 创建一个Epoll对象，使用poller_管理
// 2 调用eventfd，通过文件描述符管理，用于线程的异步唤醒
// 3 创建一个wakeupChannel对象并通过指针管理
EventLoop::EventLoop()
:   looping_(false),
    poller_(new Epoll()),
    wakeupFd_(createEventfd()),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    pwakeupChannel_(new Channel(this, wakeupFd_))
{
    if(t_loopInThisThread)
    {
//        LOG<< "Another EventLoop "<< t_loopInThisThread << "exists in this thread "<<< threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET)
    pwakeupChannel_->setReadhandler(bind(&EventLoop::handleRead, this));
    pwakeupChannel_->setConnHandler(bind(&EventLoop::handleConn, this));
    poller_->epoll_add(pwakeupChannel_, 0);//将wakeupChannel加入Epoll管理,即注册

}
void EventLoop::handleConn()
{
    updatePoller(pwakeupChannel_, 0);
}

EventLoop::~EventLoop()
{
    close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof(one));
    if(n != sizeof(one))
    {
        LOG<< "EventLoop::wakeup() writes "<< n<< " bytes instead of 8 ";
    }
}

void EventLoop::handleRead()
{
    int64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG<< "EventLoop::handleRead() reads "<< n<< " bytes instead of 8";
    }
    pwakeupChannel_->setEvent(EPOLLIN | EPOLLET);
}

void EventLoop::runInLoop(Functor&& cb)
{
    if(isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(Functor&& cb)
{
    {//这是？？
        MutexLockGuard lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}

// 开始运行：
// 在while循环中
// 1 轮询Epoll返回的Channel列表，并让Channel对象处理事件
// 2 调用doPendingFunctors，不懂这里为什么调用doPendingFunctors()什么意思？？
void EventLoop::loop()
{
    assert(!looping_);//断言
    assert(isInLoopThread());

    looping_ = true;
    quit_ = false;
    std::vector<SP_Channel> ret;

    while(!quit_)
    {
        ret.clear();
        ret = poller_->poll();//调用Epoll对象，返回一个Channel（指针）列表
        eventHandling_ = true;
        for(auto &it: ret)
            it->handleEvents();//对每个Channel对象调用handleEvents
        eventHandling_ = false;
        doPendingFunctors();
        poller_->handleExpired();
    }
    looping_ = false;
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(size_t i = 0; i < functors.size(), i++)
    {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}

