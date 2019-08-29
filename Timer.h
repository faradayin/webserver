//
// Created by Feng on 2019/8/12.
//

#ifndef WEBSERVER_TIMER_H
#define WEBSERVER_TIMER_H

#include "HttpData.h"
#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include "unistd.h"
#include <memory>
#include <queue>
#include <deque>

class HttpData;

class TimerNode
{
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    ~TimerNode()；
    TimerNode(TimerNode &tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted() {deleted_ = true;}
    bool isDeleted() const {return deleted_;}
    size_t getExpTime() const {return expiredTime_;}

private:
    bool deleted_;
    size_t expiredTime_;
    std::shared_ptr<HttpData> SPHttpData;
};

//函数对象，用于比较TimerNode
struct TimerCmp
{
    //这里定义了运算符重载函数
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const
    {
        return a->getExpTime() > b->getExpTime();
    }
};

//用一个优先队列管理TimerNode
class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
};

#endif //WEBSERVER_TIMER_H
