//
// Created by Feng on 2019/8/18.
//

#ifndef WEBSERVER_COUNTDOWNLATCH_H
#define WEBSERVER_COUNTDOWNLATCH_H

#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

class CountDownLatch : noncopyable
{
private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;

public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();

};

#endif //WEBSERVER_COUNTDOWNLATCH_H
