cmake_minimum_required(VERSION 3.5)
project(webserver)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserver main.cpp Server.h EventLoop.h Channel.h Server.cpp Channel.cpp EventLoop.cpp Epoll.h Epoll.cpp HttpData.h HttpData.cpp Util.h Util.cpp Timer.h Timer.cpp EventLoopThread.h EventLoopThread.cpp EventLoopThreadPool.h EventLoopThreadPool.cpp ThreadPool.h ThreadPool.cpp base/noncopyable.h base/Thread.h base/Condition.h base/MutexLock.h base/CountDownLatch.h base/CountDownLatch.cpp base/CurrentThread.h base/Thread.cpp)