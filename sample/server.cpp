#include <stdio.h>
#include <stdio.h>
#include "inetwork.h"

class server : public imanager
{
public:
    void run()
    {
        network_ = create_network();
        network_->listen(this, "127.0.0.1", 8086);

        while (!closed_)
            network_->update(10);
    }

    virtual void on_accept(int number, int error)
    {
        printf("on_accept number: %d, error: %d\n", number, error);
    }

    virtual void on_closed(int number, int error)
    {
        closed_ = true;
        printf("on_closed number: %d, error: %d\n", number, error);
    }

    virtual void on_package(int number, char* data, int len)
    {
        printf("number: %d, recv: %s\n", number, data);
        network_->send(number, data, len);
    }

private:
    inetwork* network_ = nullptr;
    bool closed_ = false;
};

int main()
{
    server app;
    app.run();
    printf("exit\n");
    return 0;
}
