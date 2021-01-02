#pragma once

enum {
    MSG_NONE,
    MSG_LOGIN,
    MSG_JOIN_GAME,
};

struct message {
    int msgid;
    char body[0];
};

struct login_req {
    char name[32];
};

struct login_rsp {
    int result;
    int userid;
    char name[32];
};

struct join_game_req {
    int gameid;
};

struct join_game_rsp {
    int result;
};

struct join_game_ntf {
    int userid;
    char name[32];
};

struct game_status {

};