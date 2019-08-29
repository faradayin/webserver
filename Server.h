//
// Created by Feng on 2019/8/5.
//
//我根本就不是一个目标导向的人；
#ifndef WEBSERVER_SERVER_H
#define WEBSERVER_SERVER_H

#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <memory>

class Server
{
private:
    int port_;//端口号
    int listenFd_;//welcome socket描述符
    bool started_;
    int threadNum_;
    static const int MAXFDS = 100000;

    EventLoop *loop_;//一个EventLoop对象
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;//线程池，智能指针管理
    std::shared_ptr<Channel> acceptChannel_;//Channel维护一个socket连接,智能指针管理


public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server(){}

    void start();//运行！
    void handNewConn();
    void handThisConn() {loop_->updatePoller(acceptChannel_);}
    EventLoop* getLoop() const {return loop_;}

};
#endif //WEBSERVER_SERVER_H
