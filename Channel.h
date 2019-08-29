//
// Created by Feng on 2019/8/5.
//

// Channel类：Channel是Reactor结构中的“事件”，它自始至终都属于一个EventLoop，负责一个文件描述符的IO事件，
// 在Channel类中保存这IO事件的类型以及对应的回调函数，当IO事件发生时，最终会调用到Channel类中的回调函数。
// 因此，程序中所有带有读写时间的对象都会和一个Channel关联，包括loop中的eventfd，listenfd，HttpData等

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H

#include "Timer.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>

typedef std::shared_ptr<Channel> SP_Channel;

//一个Channel对象
// 其中的fd_管理一个socket
// 其中revents_为Epoll标识，用于Epoll下，调用相应的读/写函数
// 其中的loop指针，指向EventLoop对象，为什么Channel对象要保存loop指针？？
class Channel
{
private:
    //function wrapper， Instances of std::function can store, copy, and invoke any Callable target
    typedef std::function<void()> CallBack;//理解为函数的类
    EventLoop *loop_;//包含一个EventLoop类型的指针

    //持有该Channel的HttpData对象
    std::weak_ptr<HttpData> holder_;

    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents_;


    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;


    //私有函数
    int parse_URL();
    int parse_Headers();
    int analysisRequest();


public:
    Channel(EventLoop *loop);
    Channel(EventLoop *loop, int fd);
    ~Channel();
    int getFd();
    void setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder)
    {
        holder_ = holder;
    }
    std::shared_ptr<HttpData> getHolder()
    {
        std::shared_ptr<HttpData> ret(holder_.lock());//锁？？
        return ret;
    }

    //在初始化HttpData对象时，才会调用setReadhandler/setWriteHandler/setConnHandler
    void setReadhandler(CallBack &&readHandler)
    {
        readHandler_ = readHandler;
    }
    void setWriteHandler(CallBack &&writeHandler)
    {
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler)
    {
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler)
    {
        connHandler_ = connHandler;
    }

    //在EventLoop::loop()中首先获取活跃的Channel列表，然后逐个调用该函数
    //处理事件，事件由revents标识，这里根据标识调用不同的handler函数
    void handleEvents()//？？
    {
        events_ = 0;
        if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
        {
            events_ = 0;
            return;
        }
        //判断revents_ 某些位的值， 调用某回调函数
        if((revents_ & EPOLLERR))
        {
            if(errorHandler_)
                errorHandler_();
            events_ = 0;
            return;
        }
        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        {
            handleRead();
        }
        if(revents_ & EPOLLOUT)
        {
            handleWrite();
        }
        handleConn();
    }

    void handleConn();
    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string msg);


    void setRevents(__uint32_t ev)
    {
        revents_ = ev;
    }

    void setEvents(__uint_32 ev)
    {
        events_ = ev;
    }

    __uint32_t& getEvents()//返回的是events_的引用
    {
        return events_;
    }

    bool EqualAndUpdateLastEvents()//？？
    {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }

    __uint32_t getLastEvents()
    {
        return lastEvents_;
    }



};




#endif //WEBSERVER_CHANNEL_H
