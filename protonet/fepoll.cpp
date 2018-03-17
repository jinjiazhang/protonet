#include "fepoll.h"

fepoll::fepoll()
{

}

fepoll::~fepoll()
{

}

bool fepoll::init()
{
    return true;
}

void fepoll::release()
{
    delete this;
}

int fepoll::update(int timeout)
{
    return 0;
}

int fepoll::add_event(ihandler* handler, socket_t fd, int events)
{
    return 0;
}

int fepoll::del_event(ihandler* handler, socket_t fd, int events)
{
    return 0;
}
