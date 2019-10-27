//
// Created by Feng on 2019/8/30.
//
#include "LogFile.h"
#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
using namespace std;
//logFile对象先往AppendFile对象里写，写了足够次数再让AppendFile刷新缓冲区
LogFile::LogFile(const string &basename, int flushEveryN = 1024)
:   basename_(basename),
    flushEveryN_(flushEveryN),
    count_(0),
    mutex_(new MutexLock)
{
    file_.reset(new AppendFile(basename));//缓冲结构就在这里，在堆里新建一个AppendFile对象
}

LogFile::~LogFile() {}

void LogFile::append(const char* logline, int len)
{
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush()
{
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char *logline, int len)
{
    file_->append(logline, len);
    ++count_;
    if(count_ >= flushEveryN_)
    {
        count_  = 0;
        file_->flush();
    }
}