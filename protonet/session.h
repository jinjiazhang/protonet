#ifndef _JINJIAZHANG_SESSION_H_
#define _JINJIAZHANG_SESSION_H_

#include "network.h"
#include "buffer.h"

struct ibacker
{
    virtual void on_closed(int number, int error) = 0;
    virtual void on_package(int number, char* data, int len) = 0;
};

class session : public iobject, ihandler
{
public:
    session(network* instance, ibacker* backer);
    ~session();

    bool init(socket_t fd);

    virtual void on_event(int events);
    virtual void send(char* data, int len);
    virtual void close();

private:
    void on_readable();
    void on_writable();
    void on_error(int error);
    void dispatch();

private:
    socket_t fd_;
    ibacker* backer_;
    network* network_;

    buffer recvbuf_;
    buffer sendbuf_;
};

#endif