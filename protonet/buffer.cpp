#include "buffer.h"

buffer::buffer()
{
    lenght_ = 0xFFFF;
    capacity_ = 0;
    buffer_ = NULL;
    begin_ = NULL;
    end_ = NULL;
}

buffer::~buffer()
{
    if (buffer_) 
    {
        delete[] buffer_;
    }
}

void buffer::prepare()
{
    if (capacity_ < lenght_)
    {
        buffer_ = new char[lenght_];
        begin_ = buffer_;
        end_ = begin_;
        capacity_ = lenght_;
    }
}

char* buffer::data()
{
    prepare();
    return begin_;
}

int buffer::size()
{
    return end_ - begin_;
}

int buffer::space()
{
    prepare();
    return capacity_ - (end_ - buffer_);
}

void buffer::resize(int size)
{

}

bool buffer::push_data(char* data, int len)
{
    if (len > space())
    {
        return false;
    }

    memcpy(end_, data, len);
    end_ += len;
    return true;
}

bool buffer::push_data(iovec *iov, int cnt, int ignore)
{
    return true;
}

bool buffer::pop_data(int len)
{
    if (len > size())
    {
        return false;
    }

    begin_ += len;
    return true;
}

bool buffer::pop_space(int len)
{
    if (len > space())
    {
        return false;
    }

    end_ += len;
    return true;
}

void buffer::trim_data()
{
    int used = size();
    if (used > 0)
    {
        memmove(buffer_, begin_, used);
    }

    begin_ = buffer_;
    end_ = begin_ + used;
}