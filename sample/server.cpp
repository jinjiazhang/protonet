﻿#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <map>

#include "inetwork.h"
#include "proto.h"
#include "utils.h"

class server : public imanager {
    class gobang;
    class player {
    public:
        player(server* svr) {
            server_ = svr;
        }

        void setup(int userid, int netid, char* name) {
            userid_ = userid;
            netid_ = netid;
            strcpy(name_, name);
        }

        void set_game(gobang* game) {
            game_ = game;
        }

        void send_message(int msgid, void* body, int len) {
            server_->send_message(netid_, msgid, body, len);
        }

    private:
        friend class server;
        server* server_ = nullptr;
        gobang* game_ = nullptr;
        
        int userid_ = 0;
        int netid_ = 0;
        char name_[32];
    };

    class gobang {
    public:
        gobang(server* svr) {
            server_ = svr;
        }

        void setup(int gameid) {
            gameid_ = gameid;
        }

        int join(player* role) {
            assert(players_.find(role->userid_) == players_.end());
            role->set_game(this);
            if (player1 == nullptr) {
                player1 = role;
            }
            else if (player2 == nullptr) {
                player2 = role;
            }

            join_ntf ntf = { 0 };
            ntf.userid = role->userid_;
            strcpy(ntf.name, role->name_);
            broadcast(MSG_JOIN_NTF, &ntf, sizeof(ntf));

            if (state_ == STATE_WAITING && player1 && player2) {
                state_ = STATE_PLAYING;
                sync_status();
            }

            players_.insert(std::make_pair(role->userid_, role));
            return 0;
        }

        int quit(player* role) {
            assert(players_.find(role->userid_) != players_.end());
            role->set_game(nullptr);
            players_.erase(role->userid_);

            quit_ntf ntf = { 0 };
            ntf.userid = role->userid_;
            strcpy(ntf.name, role->name_);
            broadcast(MSG_QUIT_NTF, &ntf, sizeof(ntf));

            if (role == player1 || role == player2) {
                player1 = player2 = nullptr;
                state_ = STATE_ABORTED;
                sync_status();
            }

            return 0;
        }

        int action(player* role, int row, int col) {
            if (state_ != STATE_PLAYING) {
                return -1001;
            }
            
            player* current = (hand_num % 2 == 0) ? player1 : player2;
            if (role != current) {
                return -1002;
            }

            if (!check_valid(chesses_, row, col)) {
                return -1003;
            }

            int index = (row - 1) * 15 + (col - 1);
            chesses_[index] = ++hand_num;

            action_ntf ntf = { 0 };
            ntf.userid = current->userid_;
            strcpy(ntf.name, current->name_);
            ntf.row = row;
            ntf.col = col;
            broadcast(MSG_ACTION_NTF, &ntf, sizeof(ntf));

            if (check_result(chesses_, row, col)) {
                state_ = STATE_FINISH;
            }
            sync_status();
            return 0;
        }

        void broadcast(int msgid, void* body, int len) {
            auto it = players_.begin();
            for (; it != players_.end(); ++it) {
                it->second->send_message(msgid, body, len);
            }
        }

        void sync_status(player* role = nullptr) {
            status_ntf ntf = { 0 };
            ntf.gameid = gameid_;
            ntf.state = state_;
            memcpy(ntf.chesses, chesses_, sizeof(ntf.chesses));
            if (state_ == STATE_PLAYING) {
                player* current = (hand_num % 2 == 0) ? player1 : player2;
                ntf.userid = current->userid_;
                strcpy(ntf.name, current->name_);
            }
            else if (state_ == STATE_FINISH) {
                player* winner = (hand_num % 2 == 1) ? player1 : player2;
                ntf.userid = winner->userid_;
                strcpy(ntf.name, winner->name_);
            }

            if (role != nullptr) {
                role->send_message(MSG_STATUS_NTF, &ntf, sizeof(ntf));
            }
            else {
                broadcast(MSG_STATUS_NTF, &ntf, sizeof(ntf));
            }
        }

    private:
        friend class server;
        server* server_ = nullptr;
        int gameid_ = 0;
        int state_ = STATE_WAITING;
        int hand_num = 0;
        char chesses_[255] = { 0 };
        player* player1 = nullptr;
        player* player2 = nullptr;
        std::map<int, player*> players_;
    };

public:
    void run() {
        network_ = create_network();
        network_->listen(this, "127.0.0.1", 8086);
        printf("服务器已启动，等待客户端连接\n");

        while (!closed_)
            network_->update(10);
    }

    void send_message(int netid, int msgid, void* body, int len) {
        iobuf bufs[2];
        bufs[0].data = &msgid;
        bufs[0].len = sizeof(msgid);
        bufs[1].data = body;
        bufs[1].len = len;
        network_->sendv(netid, bufs, 2);
    }

    void on_accept(int netid, int error) override {
        clients_.insert(std::make_pair(netid, nullptr));
        printf("连接[%d] 已接受\n", netid);
    }

    void on_closed(int netid, int error) override {
        printf("连接[%d] 已关闭\n", netid);
        auto it = clients_.find(netid);
        assert(it != clients_.end());
        if (it->second != nullptr) {
            on_logout(it->second);
            delete it->second;
        }

        clients_.erase(netid);
        if (clients_.size() == 0) {
            closed_ = true;
        }
    }

    void on_logout(player* role) {
        if (role->game_ != nullptr)
        {
            gobang* game = role->game_;
            game->quit(role);
            printf("玩家[%d]-%s 退出游戏(%d)\n", role->userid_, role->name_, game->gameid_);

            if (game->players_.size() == 0) {
                on_dissolve(game);
            }
        }
        assert(players_.find(role->userid_) != players_.end());
        players_.erase(role->userid_);
    }

    void on_dissolve(gobang* game) {
        printf("房间[%d] 已解散\n", game->gameid_);
        games_.erase(game->gameid_);
        delete game;
    }

    void on_package(int netid, char* data, int len) override {
        message* _msg = (message*)data;
        auto it = clients_.find(netid);
        assert(it != clients_.end());

        player* role = it->second;
        switch (_msg->msgid) {
        case MSG_LOGIN_REQ: {
            assert(role == nullptr);
            login_req* req = (login_req*)_msg->body;
            role = new player(this);
            role->setup(++last_userid_, netid, req->name);
            it->second = role;
            players_.insert(std::make_pair(role->userid_, role));

            login_rsp rsp = { 0 };
            rsp.userid = role->userid_;
            strcpy(rsp.name, role->name_);
            send_message(netid, MSG_LOGIN_RSP, &rsp, sizeof(rsp));
            printf("玩家[%d]-%s 登录成功\n", role->userid_, role->name_);
            break;
        }
        case MSG_JOIN_REQ: {
            assert(role != nullptr);
            assert(role->game_ == nullptr);
            join_req* req = (join_req*)_msg->body;
            assert(req->gameid > 0);
            auto it = games_.find(req->gameid);
            gobang* game = it != games_.end() ? it->second : nullptr;
            if (game == nullptr) {
                game = new gobang(this);
                game->setup(req->gameid);
                games_.insert(std::make_pair(game->gameid_, game));
            }

            join_rsp rsp = { 0 };
            rsp.result = game->join(role);
            assert(rsp.result == 0);
            send_message(netid, MSG_JOIN_RSP, &rsp, sizeof(rsp));
            game->sync_status(role);
            printf("玩家[%d]-%s 加入游戏(%d)\n", role->userid_, role->name_, game->gameid_);
            break;
        }
        case MSG_ACTION_REQ: {
            assert(role != nullptr);
            assert(role->game_ != nullptr);
            action_req* req = (action_req*)_msg->body;
            gobang* game = role->game_;
            action_rsp rsp = { 0 };
            rsp.result = game->action(role, req->row, req->col);
            send_message(netid, MSG_ACTION_RSP, &rsp, sizeof(rsp));
            break;
        }
        default: {
            printf("未知的消息id(%d)\n", _msg->msgid);
            break;
        }
        }
    }

private:
    inetwork* network_ = nullptr;
    bool closed_ = false;
    int last_userid_ = 1000;
    std::map<int, player*> clients_;
    std::map<int, player*> players_;
    std::map<int, gobang*> games_;
};

int main() {
    server app;
    app.run();
    printf("exit\n");
    return 0;
}
