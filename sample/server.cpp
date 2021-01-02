#include <stdio.h>
#include <string>
#include <assert.h>
#include <map>

#include "inetwork.h"
#include "proto.h"

class server : public imanager {
    class gobang;
    class player {
    public:
        player(server* svr) {
            server_ = svr;
        }

        void setup(int userid, int number, char* name) {
            userid_ = userid;
            number_ = number;
            name_ = name;
        }

        void send_message(int msgid, void* body, int len) {
            server_->send_message(number_, msgid, body, len);
        }

    private:
        friend class server;
        server* server_ = nullptr;
        gobang* game_ = nullptr;
        
        int userid_ = 0;
        int number_ = 0;
        std::string name_;
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
            return 0;
        }

        int quit(player* role) {
            return 0;
        }
    private:
        friend class server;
        server* server_ = nullptr;
        int gameid_;
    };

public:
    void run() {
        network_ = create_network();
        network_->listen(this, "127.0.0.1", 8086);
        printf("服务器启动成功\n");

        while (!closed_)
            network_->update(10);
    }

    void send_message(int number, int msgid, void* body, int len) {
        iobuf bufs[2];
        bufs[0].data = &msgid;
        bufs[0].len = sizeof(msgid);
        bufs[1].data = body;
        bufs[1].len = len;
        network_->sendv(number, bufs, 2);
    }

    void on_accept(int number, int error) override {
        clients_.insert(std::make_pair(number, nullptr));
        printf("连接[%d] 已接受\n", number);
    }

    void on_closed(int number, int error) override {
        clients_.erase(number);
        printf("连接[%d] 已关闭\n", number);

        auto it = players_.find(number);
        if (it != players_.end()) {
            on_logout(it->second);
        }

        if (clients_.size() == 0) {
            closed_ = true;
        }
    }

    void on_logout(player* role) {

    }

    void on_package(int number, char* data, int len) override {
        message* _msg = (message*)data;
        auto it = clients_.find(number);
        assert(it != clients_.end());

        player* role = it->second;
        switch (_msg->msgid) {
        case MSG_LOGIN: {
            assert(role == nullptr);
            login_req* req = (login_req*)_msg->body;
            role = new player(this);
            role->setup(++last_userid_, number, req->name);
            players_.insert(std::make_pair(role->userid_, role));
            it->second = role;

            login_rsp rsp = { 0 };
            rsp.userid = role->userid_;
            strcpy(rsp.name, role->name_.c_str());
            send_message(number, MSG_LOGIN, &rsp, sizeof(rsp));
            printf("玩家[%d]-%s 登录成功\n", role->userid_, role->name_.c_str());
            break;
        }
        case MSG_JOIN_GAME: {
            assert(role != nullptr);
            assert(role->game_ == nullptr);
            join_game_req* req = (join_game_req*)_msg->body;
            assert(req->gameid > 0);
            auto it = games_.find(req->gameid);
            gobang* game = it != games_.end() ? it->second : nullptr;
            if (game == nullptr) {
                game = new gobang(this);
                game->setup(req->gameid);
                games_.insert(std::make_pair(game->gameid_, game));
            }

            join_game_rsp rsp = { 0 };
            rsp.result = game->join(role);
            assert(rsp.result == 0);
            send_message(number, MSG_JOIN_GAME, &rsp, sizeof(rsp));
            printf("玩家[%d]-%s 加入游戏(%d)\n", role->userid_, role->name_.c_str(), game->gameid_);
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
