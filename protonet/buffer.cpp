#include "buffer.h"

buffer::buffer()
{
    preseted_ = 8096;
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
    if (buffer_ == NULL)
    {
        buffer_ = new char[preseted_];
        begin_ = buffer_;
        end_ = begin_;
        capacity_ = preseted_;
    }
	else if (capacity_ != preseted_)
	{
		char* preset = new char[preseted_];
		int used = end_ - begin_;
		if (used > 0)
		{
			assert(preseted_ >= used);
			memcpy(preset, begin_, used);
		}
		delete[] buffer_;
		buffer_ = preset;
		begin_ = buffer_;
		end_ = begin_ + used;
		capacity_ = preseted_;
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

bool buffer::expand(int size)
{
	preseted_ = preseted_ > size ? preseted_ : size;
	return true;
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
    int used = end_ - begin_;
    if (used > 0)
    {
        memmove(buffer_, begin_, used);
    }

    begin_ = buffer_;
    end_ = begin_ + used;
}