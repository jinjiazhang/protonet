#include "connector.h"

connector::connector(network* instance, imanager* manager)
{
    network_ = instance;
    manager_ = manager;
    connectfd_ = -1;
    session_ = NULL;
}

connector::~connector()
{

}

bool connector::connect(const char* ip, int port)
{
    connectfd_ = net_connect(ip, port);
    if (connectfd_ < 0)
    {
        return false;
    }

    network_->add_event(this, connectfd_, EVENT_WRITE);
    return true;
}

void connector::on_closed(int number, int error)
{
    manager_->on_closed(get_number(), error);
}

void connector::on_package(int number, char* data, int len)
{
    manager_->on_package(get_number(), data, len);
}

void connector::on_event(int events)
{
    assert(events & EVENT_WRITE);
    int error = get_socket_err(connectfd_);
    if (error != 0)
    {
        close_socket(connectfd_);
        manager_->on_accept(get_number(), error);
        return;
    }

    network_->del_event(this, connectfd_, EVENT_WRITE);
    session_ = new session(network_, this);
    if (!session_->init(connectfd_))
    {
        delete session_;
        return;
    }

    network_->add_object(session_);
    manager_->on_accept(get_number(), 0);
}

void connector::send(char* data, int len)
{
    if (session_ != NULL)
    {
        session_->send(data, len);
    }
}

void connector::close()
{
    
}