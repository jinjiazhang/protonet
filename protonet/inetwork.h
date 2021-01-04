#ifndef _JINJIAZHANG_INETWORK_H_
#define _JINJIAZHANG_INETWORK_H_

enum
{
    NET_ERROR_NONE = 0,
    NET_INNER_ERROR = -100,
    NET_HEAD_ERROR = -101,
    NET_BODY_OVERSIZE = -102, // recv large package
    NET_BUFF_OVERSIZE = -103, // send buffer full
};

struct iobuf
{
    const void* data;
    int len;
};

struct imanager
{
    virtual void on_accept(int netid, int error) = 0;
    virtual void on_closed(int netid, int error) = 0;
    virtual void on_package(int netid, char* data, int len) = 0;
};

struct inetwork
{
    virtual int update(int timeout) = 0;
    virtual int listen(imanager* manager, const char* ip, int port) = 0;
    virtual int connect(imanager* manager, const char* ip, int port) = 0;
    virtual void send(int netid, const void* data, int len) = 0;
    virtual void sendv(int netid, iobuf bufs[], int count) = 0;
    virtual void close(int netid) = 0;
    virtual void release() = 0;
};

inetwork* create_network();

#endif