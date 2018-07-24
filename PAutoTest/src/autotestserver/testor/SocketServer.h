#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <string>
#include <memory>
#include <map>

class Handler;
class HandlerManager;

class SocketServer
{
private:
    std::map<int, std::shared_ptr<Handler> > handlers;
    std::shared_ptr<HandlerManager> manager_;
    int _listenfd;

public:
    SocketServer();
    virtual ~SocketServer();

    bool start();
    bool waitToClient(int timeout);
    bool SendMsg(char* command, int len);
};

#endif /* SOCKETSERVER_H */
