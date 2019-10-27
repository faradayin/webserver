//
// Created by Feng on 2019/8/18.
//

#ifndef WEBSERVER_MUTEXLOCK_H
#define WEBSERVER_MUTEXLOCK_H

#include "noncopyable.h"
#include <pthread.h>
#include <cstdio>
//mutex的创建、销毁、上锁、解锁
class MutexLock : noncopyable
{
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex, NULL);
    }

    ~MutexLock()
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_t *get()
    {
        return &mutex;
    }

private:
    pthread_mutex_t mutex;
    friend class Condition;
};

class MutexLockGuard : noncopyable
{
public:
    //explicit构造函数
    explicit MutexLockGuard(MutexLock &_mutex): mutex(_mutex)
    {
        mutex.lock();
    }

    ~MutexLockGuard()
    {
        mutex.unlock();
    }

private:
    MutexLock &mutex;
};

#endif //WEBSERVER_MUTEXLOCK_H
