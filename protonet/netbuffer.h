#ifndef _JINJIAZHANG_NETBUFFER_H_
#define _JINJIAZHANG_NETBUFFER_H_

#include "platform.h"

class netbuffer
{
public:
    netbuffer();
    ~netbuffer();
    
    char* data();
    int size();
    int space();
    
    void prepare();
    void resize(int size);
    bool push_data(char* data, int len);
    bool push_data(iovec *iov, int cnt, int ignore);
    bool pop_data(int len);
    bool pop_space(int len);
    void trim_data();    

private:
    int lenght_;
    int capacity_;
    char* buffer_;
    char* begin_;
    char* end_;
};

#endif