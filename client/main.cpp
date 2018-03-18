#include <iostream>
#include "inetwork.h"

class clinet : public imanager
{
public:
    clinet()
    {
        network_ = NULL;
        closed_ = false;
        connect_ = 0;
    }

    void run()
    {
        network_ = create_network();
        connect_ = network_->connect(this, "127.0.0.1", 8086);
        if (connect_ <= 0)
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
        if (error != 0)
        {
            closed_ = true;
            return;
        }

		for (int serial = 1; serial < 10000; serial++)
		{
			network_->send(number, (char*)&serial, sizeof(serial));
		}
    }

    virtual void on_closed(int number, int error)
    {
        closed_ = true;
    }

    virtual void on_package(int number, char* data, int len)
    {
        int serial = *(int*)data;
        printf("number: %d, recv: %d, send: %d\n", number, serial, serial + 1);
		serial++;
        network_->send(number, (char*)&serial, sizeof(serial));
    }

private:
    inetwork* network_;
    bool closed_;
    int connect_;
};

int main()
{
    clinet app;
    app.run();
    printf("exit\n");
    return 0;
}