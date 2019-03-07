#include <stdio.h>
#include <iostream>
#include "inetwork.h"

class clinet : public imanager
{
public:
    void run()
    {
        network_ = create_network();
        network_->connect(this, "127.0.0.1", 8086);

        while (!closed_)
            network_->update(10);
    }

    void on_accept(int number, int error) override
    {
        const char* data = "hello world!";
        network_->send(number, (char*)data, strlen(data) + 1);
    }

    void on_closed(int number, int error) override
    {
        closed_ = true;
    }

    void on_package(int number, char* data, int len) override
    {
        printf("number: %d, recv: %s\n", number, data);
    }

private:
    inetwork* network_ = nullptr;
    bool closed_ = false;
};

int main()
{
    clinet app;
    app.run();
    printf("exit\n");
    return 0;
}
