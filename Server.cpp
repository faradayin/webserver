//
// Created by Feng on 2019/8/5.
//
#include "Server.h"
#include "base/Logging.h"
#include "Util.h"
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

Server::Server(EventLoop *loop, int threadNum, int port)
//默认参数
:   loop_(loop),
    threadNum_(threadNum),
    eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
    started_(false),
    acceptChannel_(new Channel(loop_)),
    port_(port),
    listenFd_(socket_bind_listen(loop_))//获取监听socket描述符
{

    acceptChannel_->setFd(listenFd_);
    handle_for_sigpipe();
    if(setsocketNonBlocking(listenFd_) < 0)//非阻塞
    {
        perror("set socket non block failed");
        abort();
    }
}

// Server开始运行：
// 1 开启线程池
// 2 设置acceptChannel相关参数，包括相应的Epoll标识，回调函数
// 3 将acceptChannel加入Epoll管理
void Server::start()
{
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadhandler(bind(&Server::handNewConn, this));
    acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn()
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while((accept_fd = accept(listenFd_, （struct sockaddr*)&client_addr, &client_addr_len) > 0)
    {
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        LOG<< "New connection from "<< inet_ntoa(client_addr.sin_addr)<< ":"<< ntohs(client_addr.sin_port);
        //最大连接数
        if(accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }
        //设置非阻塞模式
        if(setSocketNonBlocking(accept_fd) < 0)
        {
            LOG<< "Set non block failed!";
            return;
        }
        setSocketNodelay(accept_fd);

        shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop->queueInLoop(std::bind(&HttpData::newEvent, req_info));
    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}
