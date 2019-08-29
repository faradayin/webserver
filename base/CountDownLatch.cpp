//
// Created by Feng on 2019/8/18.
//
#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
:   mutex_(),
    condition_(mutex_),
    count_(count)
{}

void CountDownLatch::wait()
{
    MutexLockGuard lock(mutex_);
    while(count_ > 0)
        condition_.wait();
}

void CountDownLatch::countDown()
{
    MutexLockGuard lock(mutex_);
    --count;
    if(count_ == 0)
        condition_.notifyAll();
}