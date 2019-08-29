//
// Created by Feng on 2019/8/8.
//

#ifndef WEBSERVER_UTIL_H
#define WEBSERVER_UTIL_H

#include <cstdlib>
#include <string>

ssize_t readn(int fd, void *buff, size_t);
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
ssize_t readn(int fd, std::string &inBuffer);

ssize_t writen(int fd, void *buff, size_t n);
ssize_t writen(int fd, std::string, &sbuff);

void handle_for_sigpipe();
int setSocketNonBlocking(int fd);
void setSocketNodelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);


#endif //WEBSERVER_UTIL_H