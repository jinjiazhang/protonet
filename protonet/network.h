#ifndef _JINJIAZHANG_NETWORK_H_
#define _JINJIAZHANG_NETWORK_H_

#include "inetwork.h"
#include "platform.h"

enum
{
    EVENT_READ = 0x01,
    EVENT_WRITE = 0x04,
};

struct ihandler
{
    ihandler() { events_ = 0; }
    virtual ~ihandler() { }
    int get_events() { return events_; }
    void set_events(int events) { events_ = events; }

    virtual void on_event(int events) = 0;

protected:
    int events_;
};

struct iframe
{
    virtual bool init() = 0;
    virtual void release() = 0;
    virtual int update(int timeout) = 0;
    virtual int add_event(ihandler* handler, socket_t fd, int events) = 0;
    virtual int del_event(ihandler* handler, socket_t fd, int events) = 0;
};

struct iobject
{
    iobject() { number_ = 0; }
    virtual ~iobject() { }
    int get_number() { return number_; }
    void set_number(int number) { number_ = number; }

    virtual void send(char* data, int len) = 0;
    virtual void close() = 0;

protected:
    int number_;
};

class network : public inetwork
{
public:
    network();
    ~network();

    virtual bool init();
    virtual int update(int timeout);
    virtual int listen(imanager* manager, const char* ip, int port);
    virtual int connect(imanager* manager, const char* ip, int port);
    virtual void send(int number, char* data, int len);
    virtual void close(int number);
    virtual void release();

public:
    int add_event(ihandler* handler, socket_t fd, int events);
    int del_event(ihandler* handler, socket_t fd, int events);

    int new_number();
    int add_object(iobject* object);
    int del_object(iobject* object);
    iobject* get_object(int number);

private:
    typedef std::map<int, iobject*> object_map;
    object_map objects;
    int last_number_;
    iframe* frame_;
};

#endif