#include <iostream>
#include "EventLoop.h"
#include "Server.h"
#include "base/logging.h"
#include <getopt.h>
#include <string>

int main(int argc, char *argv[])
{
    int threadNum = 4;
    int port = 80;
    std::string logPath = "./WebServer.log";
    int opt;
    const char *str = "t:l:p:";
    while((opt = getopt(argc, argv, str)) != -1)
    {
        switch(opt)
        {
            case 't':
            {
                threadNum = atoi(optarg);
                break;
            }
            case 'l':
            {
                logPath = optarg;
                if(logPath.size() < 2 || optarg[0] != '/')
                {
                    printf("logPath should start with \"/\"\n");
                    abort();//YUUNA
                }
                break;
            }
            case 'p':
            {
                port = atoi(optarg);
                break;
            }
            default:
                break;

        }
    }
    Logger::setLogFileName(logPath);
#ifndef _PTHREADS
    LOG << "_PTHREAD is not defined !";
#endif
    EventLoop mainloop;//创建一个mainloop对象
    Server myHTTPServer(&mainloop, threadNum, port);//创建一个Server对象，关联了mainloop，指定了线程数和端口号
    myHTTPServer.start();
    mainloop.loop();
    return 0;
}