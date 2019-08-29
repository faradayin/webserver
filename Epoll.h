//
// Created by Feng on 2019/8/7.
//

#ifndef WEBSERVER_EPOLL_H
#define WEBSERVER_EPOLL_H

#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>

class Epoll
{
public:
    Epoll();
    ~Epoll();
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel request);
    std::vector<SP_Channel> poll();
    std::vector<SP_Channel> getEventsRequest(int events_num);
    void add_timer(SP_Channel request_data, int timeout);
    int getEpollFd()
    {
        return epollFd_;
    }
    void handleExpired();

private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;//epoll_event列表
    SP_Channel fd2chan_[MAXFDS];//fd映射到SP_Channel
    std::shared_ptr<HttpData> fd2http_[MAXFDS];//fd映射到HttpData指针
    TimerManager timerManager_;//一个定时管理器
};


#endif //WEBSERVER_EPOLL_H
