#include <stdio.h>
#include <iostream>
#include <string>
#include "inetwork.h"

class clinet : public imanager
{
public:
    void run()
    {
        network_ = create_network();
        int number = network_->connect(this, "127.0.0.1", 8086);

        std::string content = "hello world!";
        network_->send(number, content.c_str(), content.length() + 1);

        while (!closed_)
            network_->update(10);
    }

    void on_accept(int number, int error) override
    {
        
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
