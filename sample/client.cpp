#include <stdio.h>
#include <string>
#include <assert.h>

#include "inetwork.h"
#include "proto.h"
#include "utils.h"

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

        login_req req = { 0 };
        printf("���ȸ��Լ�������֣�");
        scanf("%s", req.name);
        send_message(MSG_LOGIN_REQ, &req, sizeof(req));
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
        case MSG_LOGIN_RSP:
        {
            login_rsp* rsp = (login_rsp*)_msg->body;
            assert(rsp->result == 0);
            
            userid_ = rsp->userid;
            printf("��¼��Ϸ�ɹ������IDΪ%d\n", userid_);
            
            join_req req = { 0 };
            printf("��������Ҫ����ķ���id��");
            scanf("%d", &req.gameid);
            send_message(MSG_JOIN_REQ, &req, sizeof(req));
            break;
        }
        case MSG_JOIN_RSP:
        {
            join_rsp* rsp = (join_rsp*)_msg->body;
            assert(rsp->result == 0);
            printf("���뷿��ɹ�\n");
            break;
        }
        case MSG_JOIN_NTF:
        {
            join_ntf* ntf = (join_ntf*)_msg->body;
            printf("���[%d]-%s ���뷿��\n", ntf->userid, ntf->name);
            break;
        }
        case MSG_QUIT_NTF:
        {
            quit_ntf* ntf = (quit_ntf*)_msg->body;
            printf("���[%d]-%s �˳�����\n", ntf->userid, ntf->name);
            break;
        }
        case MSG_ACTION_RSP:
        {
            action_rsp* rsp = (action_rsp*)_msg->body;
            assert(rsp->result == 0);
            break;
        }
        case MSG_ACTION_NTF:
        {
            action_ntf* ntf = (action_ntf*)_msg->body;
            printf("���[%d]-%s ����(%d,%d)\n", ntf->userid, ntf->name, ntf->row, ntf->col);
            break;
        }
        case MSG_STATUS_NTF:
        {
            status_ntf* ntf = (status_ntf*)_msg->body;
            if (ntf->state == STATE_WAITING) {
                printf("�ȴ����ּ���\n");
                return;
            }

            if (ntf->state == STATE_ABORTED) {
                printf("�������˳�����Ϸ��ֹ\n");
                return;
            }

            draw_chesses(ntf->chesses);
            if (ntf->state == STATE_FINISH) {
                printf("��Ϸ���������[%d]-%sʤ��\n", ntf->userid, ntf->name);
                return;
            }

            if (ntf->state == STATE_PLAYING) {
                if (userid_ != ntf->userid) {
                    printf("�ȴ����[%d]-%s����\n", ntf->userid, ntf->name);
                    return;
                }
                
                action_req req = { 0 };
                while (true)
                {
                    printf("�ֵ������壬����������(x,y)��");
                    scanf("%d,%d", &req.row, &req.col);
                    if (check_valid(ntf->chesses, req.row, req.col)) {
                        break;
                    }
                    printf("���������Ч������������\n");
                }
                send_message(MSG_ACTION_REQ, &req, sizeof(req));
            }
            
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
