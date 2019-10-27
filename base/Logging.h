//
// Created by Feng on 2019/8/31.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H

#include "LogStream.h"
#include <pthread.h>
#include <string.h>
#include <string>
#include <stdio.h>

class AsyncLogging;

class Logger
{
private:
    class Impl
    {
    public:
        Impl(const char *fileName, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;

public:
    Logger(const char *fileName, int line);
    ~Logger();
    LogStream& stream() {return impl_.stream_;}

    static void setLogFileName(std::string fileName)
    {
        logFileName_ = fileName;
    }

    static std::string getLogFileName()
    {
        return logFileName_;
    }

};

#define LOG logger(__FILE__, __LINE__).stream();

#endif //WEBSERVER_LOGGING_H
