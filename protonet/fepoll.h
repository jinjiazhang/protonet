#ifndef _JINJIAZHANG_FEPOLL_H_
#define _JINJIAZHANG_FEPOLL_H_

#include "network.h"

class fepoll : public iframe
{
public:
    fepoll();
    ~fepoll();

    virtual bool init();
    virtual void release();
    virtual int update(int timeout);
    virtual int add_event(ihandler* handler, socket_t fd, int events);
    virtual int del_event(ihandler* handler, socket_t fd, int events);

private:

};

#endif