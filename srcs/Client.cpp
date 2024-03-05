
#include "../includes/Client.hpp"

Client::Client()
{
    read = false;
    enf = false;
    sread = 0;
    flag = false;
    resred = false;
    chunksize = 0;
    contentlength = 0;
    fd = 0;
}

Client & Client::operator= ( const Client & other )
{
    read = other.read;
    enf = other.enf;
    sread = other.sread;
    flag = other.flag;
    chunksize = other.chunksize;
    contentlength = other.contentlength;
    reqRes = other.reqRes;
    fd = other.fd;
    return *this;
}

Client::Client (const Client & other )
{
    *this = other; 
}