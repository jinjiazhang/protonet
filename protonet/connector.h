#ifndef _JINJIAZHANG_CONNECTOR_H_
#define _JINJIAZHANG_CONNECTOR_H_

#include "network.h"
#include "session.h"

class connector : public iobject, imanager
{
public:
    connector(network* instance, imanager* manager);
    ~connector();

    bool connect(const char* ip, int port);

	virtual void on_accept(int number, int error);
    virtual void on_closed(int number, int error);
    virtual void on_package(int number, char* data, int len);
    virtual void on_event(int events);
    virtual void send(char* data, int len);
    virtual void close();

private:
    network* network_;
    imanager* manager_;
    socket_t connectfd_;
    session* session_;
};

#endif