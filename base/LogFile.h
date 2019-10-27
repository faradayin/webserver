//
// Created by Feng on 2019/8/30.
//

#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H

#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"
#include <memory>
#include <string>

//LogFile进一步封装了FileUtil，并设置了一个循环次数，没过这么多次就flush一次。
class LogFile : noncopyable
{
public:
    logFile(const std::string &basename, int flushEveryN = 1024);
    ~logFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);
    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};
#endif //WEBSERVER_LOGFILE_H
