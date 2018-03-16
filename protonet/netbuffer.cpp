#include "netbuffer.h"

netbuffer::netbuffer()
{
    lenght_ = 0xFFFF;
    capacity_ = 0;
    buffer_ = NULL;
    begin_ = NULL;
    end_ = NULL;
}

netbuffer::~netbuffer()
{
    if (buffer_) 
    {
        delete[] buffer_;
    }
}

void netbuffer::prepare()
{
    if (capacity_ < lenght_)
    {
        buffer_ = new char[lenght_];
        begin_ = buffer_;
        end_ = begin_;
        capacity_ = lenght_;
    }
}

char* netbuffer::data()
{
    prepare();
    return begin_;
}

int netbuffer::size()
{
    return end_ - begin_;
}

int netbuffer::space()
{
    prepare();
    return capacity_ - (end_ - buffer_);
}

void netbuffer::resize(int size)
{

}

bool netbuffer::push_data(char* data, int len)
{
    prepare();
    if (len > space())
    {
        return false;
    }

    memcpy(end_, data, len);
    end_ += len;
    return true;
}

bool netbuffer::push_data(iovec *iov, int cnt, int ignore)
{
    return true;
}

bool netbuffer::pop_data(int len)
{
    if (len > size())
    {
        return false;
    }

    begin_ += len;
    return true;
}

bool netbuffer::pop_space(int len)
{
    if (len > space())
    {
        return false;
    }

    end_ += len;
    return true;
}

void netbuffer::trim_data()
{
    int used = size();
    if (used > 0)
    {
        memmove(buffer_, begin_, used);
    }

    begin_ = buffer_;
    end_ = begin_ + used;
}