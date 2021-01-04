#include <stdio.h>
#include <string.h>
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
        printf("连接服务器成功\n");

        login_req req = { 0 };
        printf("请先给自己起个名字：");
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
            printf("登录游戏成功，你的ID为%d\n", userid_);
            
            join_req req = { 0 };
            printf("请输入你要加入的房间id：");
            scanf("%d", &req.gameid);
            send_message(MSG_JOIN_REQ, &req, sizeof(req));
            break;
        }
        case MSG_JOIN_RSP:
        {
            join_rsp* rsp = (join_rsp*)_msg->body;
            assert(rsp->result == 0);
            printf("加入房间成功\n");
            break;
        }
        case MSG_JOIN_NTF:
        {
            join_ntf* ntf = (join_ntf*)_msg->body;
            printf("玩家[%d]-%s 加入房间\n", ntf->userid, ntf->name);
            break;
        }
        case MSG_QUIT_NTF:
        {
            quit_ntf* ntf = (quit_ntf*)_msg->body;
            printf("玩家[%d]-%s 退出房间\n", ntf->userid, ntf->name);
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
            printf("玩家[%d]-%s 下了(%d,%d)\n", ntf->userid, ntf->name, ntf->row, ntf->col);
            break;
        }
        case MSG_STATUS_NTF:
        {
            status_ntf* ntf = (status_ntf*)_msg->body;
            if (ntf->state == STATE_WAITING) {
                printf("等待对手加入\n");
                return;
            }

            if (ntf->state == STATE_ABORTED) {
                printf("有棋手退出，游戏中止\n");
                return;
            }

            draw_chesses(ntf->chesses);
            if (ntf->state == STATE_FINISH) {
                printf("游戏结束，玩家[%d]-%s胜利\n", ntf->userid, ntf->name);
                return;
            }

            if (ntf->state == STATE_PLAYING) {
                if (userid_ != ntf->userid) {
                    printf("等待玩家[%d]-%s下棋\n", ntf->userid, ntf->name);
                    return;
                }
                
                action_req req = { 0 };
                while (true)
                {
                    printf("轮到你下棋，请输入坐标(x,y)：");
                    scanf("%d,%d", &req.row, &req.col);
                    if (check_valid(ntf->chesses, req.row, req.col)) {
                        break;
                    }
                    printf("你的输入无效，请重新输入\n");
                }
                send_message(MSG_ACTION_REQ, &req, sizeof(req));
            }
            
            break;
        }
        default:
        {
            printf("未知的消息id(%d)\n", _msg->msgid);
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
