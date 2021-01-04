#pragma once

enum {
    MSG_NONE,
    MSG_LOGIN_REQ,
    MSG_LOGIN_RSP,
    MSG_JOIN_REQ,
    MSG_JOIN_RSP,
    MSG_JOIN_NTF,
    MSG_QUIT_NTF,
    MSG_ACTION_REQ,
    MSG_ACTION_RSP,
    MSG_ACTION_NTF,
    MSG_STATUS_NTF,
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

struct join_req {
    int gameid;
};

struct join_rsp {
    int result;
    int gameid;
};

struct join_ntf {
    int userid;
    char name[32];
};

struct quit_ntf {
    int userid;
    char name[32];
};

struct action_req {
    int row;
    int col;
};

struct action_rsp {
    int result;
};

struct action_ntf {
    int userid;
    char name[32];
    int row;
    int col;
};

enum {
    STATE_WAITING,
    STATE_PLAYING,
    STATE_FINISH,
    STATE_ABORTED,
};

struct status_ntf {
    int gameid;
    int state;
    char chesses[255];
    int userid;
    char name[32];
};