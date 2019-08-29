//
// Created by Feng on 2019/8/7.
//
#include "Epoll.h"
#include "Util.h"
#include "base/Logging.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <queue>
#include <deque>
#include <assert.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll()
:   epollFd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(EVENTSNUM)
{
    assert(epollFd_ > 0);
}

Epoll::~Epoll() {}

//注册一个Channel至Epoll：
// 1 获取该Channel相应的fd
// 2 获取该Channel相应的HttpData
// 3 获取该Channel的events
// 4 利用epoll_ctl函数，将fd，events写入epoll
void Epoll::epoll_add(SP_Channel request, int timeout)
{
    int fd = request->getFd();//返回socket文件描述符
    if(timeout > 0)
    {
        add_timer(request, timeout);//给该Channel添加定时
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();//获取events_标识

    request->EqualAndUpdateLastEvents();
    fd2chan_[fd] = request;
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error");
        fd2chan_[fd].reset();
    }
}

//修改Channel在Epoll中的状态
void Epoll::epoll_mod(SP_Channel request, int timeout)
{
    if(timeout > 0) add_timer(request, timeout);//给该Channel添加定时
    int fd = request->getFd();
    if(!request->EqualAndUpdateLastEvents())
    {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();//获取events_标识
        if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0)
        {
            perror("epoll_mod error");
            fd2chan_[fd].reset();
        }
    }
}

void Epoll::epoll_del(SP_Channel request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) perror("epoll_del error");
    fd2chan_[fd].reset();
    fd2http_[fd].reset();
}

//获取Channel列表
std::vector<SP_Channel> Epoll::poll()
{
    while(true)
    {
        int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
        if(event_count < 0) perror("epoll wait error");
        std::vector<SP_Channel> req_data =getEventsRequest(event_count);
        if(req_data.size() > 0)
            return req_data;
    }
}

//处理超时？？
void Epoll::handleExpired()
{
    timerManager_.handleExpiredEvent();
}

//获取所有事件的描述符，生成一个活跃Channel列表并返回
std::vector<SP_Channel> Epoll::getEventsRequest(int events_num)
{
    std::vector<SP_Channel> req_data;
    for(int i=0; i<events_num; i++)
    {
        int fd = events_[i].data.fd;
        SP_Channel cur_req = fd2chan_[fd];
        if(cur_req)
        {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            req_data.push_back(cur_req);
        }
        else
            LOG<< "SP cur_req is invalid";

    }
    return req_data;
}

void Epoll::add_timer(SP_Channel request_data, int timeout)
{
    std::shared_ptr<HttpData> t = request_data->getHolder();
    if(t)
        timerManager_.addTimer(t, timeout);
    else
        LOG<< "timer add fail";
}
