#include "SocketServer.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <thread>
#include <map>
#include <iostream>
#include <sys/ioctl.h>
#include<netdb.h>
#include <fcntl.h>
#include "LOG.h"

#include "sockcomm.h"

class HandlerManager {
public:
    void add(int fd, std::shared_ptr<Handler> handler) {
        handlers[fd] = handler;
    }

    void del(int fd) {
        handlers.erase(fd);
    }

    std::map<int, std::shared_ptr<Handler> >& getHandlers() {
        return handlers;
    }

private:
    std::map<int, std::shared_ptr<Handler> > handlers;
};

class Handler {
public:
    Handler(HandlerManager* manager, int fd):_manager(manager), _fd(fd) {
        _thread = std::thread(&Handler::run, this);
        _thread.detach();
    }

    int fd() const {
        return _fd;
    }

private:
    void run() {
        ssize_t n;
        char buffer[BUF_LEN];
        while((n=readp(_fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[n] = '\0';
            LOG("=>%s\n", buffer);
        }
        _manager->del(_fd);
        close(_fd);
    }

    HandlerManager* _manager;
    int _fd;
    std::thread _thread;
};

static void runInput(HandlerManager* manager) {
    char buffer[BUF_LEN];
    while (fgets(buffer, sizeof(buffer)-1, stdin) != NULL) {
        if (0 == manager->getHandlers().size()) {
            LOG("****** no client be connected!\n");
            continue;
        }

        for (auto it = manager->getHandlers().begin(); it != manager->getHandlers().end(); ++it) {
            writep(it->first, buffer, strlen(buffer));
        }
    }
}



SocketServer::SocketServer()
    : manager_(std::shared_ptr<HandlerManager>(new HandlerManager()))
    , _listenfd(-1)
{

}

SocketServer::~SocketServer()
{
    if (-1 != _listenfd) {
        close(_listenfd);
        _listenfd = -1;
    }
}

//bool SocketServer::start()
//{
//    int err;
//    struct sockaddr_in serv_addr;

//    _listenfd = socket(AF_INET, SOCK_STREAM, 0);
//    memset(&serv_addr, '0', sizeof(serv_addr));

//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(SOCK_PORT);

//    CHECK(inet_pton(AF_INET,IP_ADDR,&serv_addr.sin_addr));
//    CHECK(connect(_listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)));

//    return true;
//}

bool SocketServer::start()
{
    bool ret = false;
    int err = 0;
    int len = sizeof(int);
    int error = -1;
    struct sockaddr_in serv_addr;

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SOCK_PORT);

    inet_aton(IP_ADDR,&serv_addr.sin_addr);

    _listenfd = socket(AF_INET, SOCK_STREAM, 0);

    unsigned long ul = 1;
    ioctl(_listenfd, FIONBIO, &ul);

    if (connect(_listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))) {
//        LOG("connect error\n");
        timeval tm;
        fd_set set;
        tm.tv_sec = TIME_OUT_TIME;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(_listenfd, &set);

        if(select(_listenfd+1, NULL, &set, NULL, &tm) > 0) {
//            LOG("select ok\n");
            getsockopt(_listenfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            if(error == 0) {
//                LOG("getsockopt ok\n");
                ret = true;
            }
            else {
//                LOG("getsockopt error\n");
                ret = false;
            }
        }
        else {
//            LOG("select error\n");
            ret = false;
        }
    }
    else {
//        LOG("connect ok\n");
        ret = true;
    }

    ul = 0;
    ioctl(_listenfd, FIONBIO, &ul);
    if(!ret)
    {
        close( _listenfd );
        LOG("Cannot Connect the server!\n");
        exit(0);
    }

    return true;
}

//bool SocketServer::waitToClient(int timeout)
//{
//    struct sockaddr_in cli_addr;
//    socklen_t clilen = sizeof(cli_addr);
//    int cfd = accept(_listenfd, (struct sockaddr*)&cli_addr, &clilen);
//    LOG("connection from %s\n", inet_ntoa(cli_addr.sin_addr));
//    if (cfd > 0) {
//        manager_->add(cfd, std::make_shared<Handler>(manager_.get(), cfd));
//    }
//    return true;
//}

bool SocketServer::SendMsg(char* command, int len)
{
    send(_listenfd, command, len, 0);

    return true;
}
