//
// Created by Feng on 2019/8/12.
//
#include "Timer.h"
#include <sys/time.h>
#include <unistd.h>
#include <queue>
/*
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
*/
TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
:   deleted_(false),
    SPHttpData(requestData)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    expiredTime_ = (((now.tv_sec%10000)*1000) + (now.tv_usec/1000)) + timeout;
}

TimerNode::~TimerNode()
{
    if(SPHttpData)
        SPHttpData->handleClose();
}

TimerNode::TimerNode(TimerNode &tn): SPHttpData(tn.SPHttpData) {}

void TimerNode::isValid()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t temp = (((now.tv_sec%10000)*1000) + (now.tv_usec/1000));
    if(temp < expiredTime_)
        return true;
    else
    {
        this->setDeleted();
        return false;
    }
}

void TimerNode::clearReq()
{
    SPHttpData.reset();
    this->setDeleted();
}

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout)
{
    SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
    timerNodeQueue.push(new_node);
    SPHttpData->linkTimer(new_node);
}

//对于被置为deleted的时间节点，会延迟到它(1)超时 或 (2)它前面的节点都被删除时，它才会被删除
void TimerManager::handleExpiredEvent()
{
    while(!timerNodeQueue.empty())
    {
        SPTimerNode ptimer_now = timerNodeQueue.top();
        if(ptimer_now->isDeleted())
            timerNodeQueue.pop();
        else if(ptimer_now->isValid() == false)
            timerNodeQueue.pop();
        else
            break;
    }
}
