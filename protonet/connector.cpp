#include "connector.h"

connector::connector(network* instance, imanager* manager)
:session(instance, manager)
{
    connectfd_ = -1;
    connected_ = false;
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

void connector::on_event(int events)
{
	if (connected_)
	{
		session::on_event(events);
		return;
	}

    assert(events & EVENT_WRITE);
    int error = get_socket_err(connectfd_);
    if (error != 0)
    {
        manager_->on_accept(number_, error);
		close_socket(connectfd_);
        return;
    }

    network_->del_event(this, connectfd_, EVENT_WRITE);
    if (!this->init(connectfd_))
    {
		manager_->on_accept(number_, -1);
		close_socket(connectfd_);
		return;
    }

	connected_ = true;
    manager_->on_accept(number_, 0);
}

void connector::send(char* data, int len)
{
    if (connected_)
    {
		session::send(data, len);
    }
}

void connector::close()
{
    
}