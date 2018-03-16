#include <stdio.h>
#include "inetwork.h"

class server : public imanager
{
public:
    server()
    {
        network_ = NULL;
        closed_ = false;
        listen_ = 0;
    }

    void run()
    {
        network_ = create_network();
        listen_ = network_->listen(this, "127.0.0.1", 8086);
        if (listen_ <= 0)
        {
            return;
        }

        while (!closed_)
        {
            network_->update(10);
        }
    }

    virtual void on_accept(int number, int error)
    {
        printf("on_accept number: %d, error: %d", number, error);
    }

    virtual void on_closed(int number, int error)
    {
        closed_ = true;
    }

    virtual void on_package(int number, char* data, int len)
    {
        int serial = *(int*)data;
        printf("number: %d, recv: %d, send: %d\n", number, serial, ++serial);
        network_->send(number, (char*)&serial, sizeof(serial));
    }

private:
    inetwork* network_;
    bool closed_;
    int listen_;
};

int main()
{
    server app;
    app.run();
    printf("exit\n");
    return 0;
}