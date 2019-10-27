//
// Created by Feng on 2019/8/30.
//

#ifndef WEBSERVER_FILEUTIL_H
#define WEBSERVER_FILEUTIL_H

#include "noncopyable.h"
#include <string>

// FileUtil是最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭文件，底层使用了标准IO，该append函数直接向文件写。
class AppendFile : noncopyable
{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();

    void append(const char *logline, const size_t len);
    void flush();

private:
    size_t wirte(const char *logline, size_t len);
    FILE* fp_;
    char buffer_[64*1024];
};


#endif //WEBSERVER_FILEUTIL_H
