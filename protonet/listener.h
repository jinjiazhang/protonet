#ifndef _JINJIAZHANG_LISTENER_H_
#define _JINJIAZHANG_LISTENER_H_

#include "network.h"
#include "session.h"

class listener : public iobject, imanager
{
public:
    listener(network* instance, imanager* manager);
    ~listener();

    bool listen(const char* ip, int port);

	virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);
    virtual void on_event(int events);
    virtual void send(char* data, int len);
    virtual void close();

private:
    network* network_;
    imanager* manager_;
    socket_t listenfd_;
};

#endif