#include <stdio.h>
#include <string>
#include <assert.h>

#include "inetwork.h"
#include "proto.h"

class clinet : public imanager
{
public:
    void run()
    {
        network_ = create_network();
        int number = network_->connect(this, "127.0.0.1", 8086);

        while (!closed_)
            network_->update(10);
    }

    void on_accept(int number, int error) override
    {
        assert(error == 0);
        number_ = number;
        printf("���ӷ������ɹ�\n");
        printf("����Լ�������֣�");
        login_req req = { 0 };
        scanf("%s", req.name);
        send_message(MSG_LOGIN, &req, sizeof(req));
    }

    void on_closed(int number, int error) override
    {
        closed_ = true;
    }

    void on_package(int number, char* data, int len) override
    {
        message* _msg = (message*)data;
        switch (_msg->msgid)
        {
        case MSG_LOGIN:
        {
            login_rsp* rsp = (login_rsp*)_msg->body;
            assert(rsp->result == 0);
            
            userid_ = rsp->userid;
            printf("��¼��Ϸ�ɹ������IDΪ%d\n", userid_);
            printf("��������Ҫ����ķ���id��");
            join_game_req req = { 0 };
            scanf("%d", &req.gameid);
            send_message(MSG_JOIN_GAME, &req, sizeof(req));
            break;
        }
        case MSG_JOIN_GAME:
        {
            join_game_rsp* rsp = (join_game_rsp*)_msg->body;
            assert(rsp->result == 0);
            printf("���뷿��ɹ�\n");
            break;
        }
        default:
        {
            printf("δ֪����Ϣid(%d)\n", _msg->msgid);
            break;
        }
        }
    }

    void send_message(int msgid, void* body, int len)
    {
        iobuf bufs[2];
        bufs[0].data = &msgid;
        bufs[0].len = sizeof(msgid);
        bufs[1].data = body;
        bufs[1].len = len;
        network_->sendv(number_, bufs, 2);
    }

private:
    inetwork* network_ = nullptr;
    bool closed_ = false;
    int number_ = 0;
    int userid_ = 0;
};

int main()
{
    clinet app;
    app.run();
    printf("exit\n");
    return 0;
}
