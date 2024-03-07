
#include "../includes/Client.hpp"

Client::Client()
{
    sfd = 0;
    fd = 0;
    chunksize = 0;
    contentlength = 0;
    sread = 0;
    start = 0;
    end = 0;
    postpath = "";
    chunkedbuff = "";
    request = "";
    requestHeader = "";
    filename = "";
    read = false;
    flag = false;
    requestclosed = false;
    enf = false;
    resred = false;
}

Client & Client::operator= ( const Client & other )
{
    sfd = other.sfd;
    fd = other.fd;
    chunksize = other.chunksize;
    contentlength = other.contentlength;
    sread = other.sread;
    start = other.start;
    end = other.end;
    postpath = other.postpath;
    chunkedbuff = other.chunkedbuff;
    request = other.request;
    requestHeader = other.requestHeader;
    filename = other.filename;
    reqRes = other.reqRes;
    read = other.read;
    flag = other.flag;
    requestclosed = other.requestclosed;
    enf = other.enf;
    resred = other.resred;
    return *this;
}

Client::Client (const Client & other )
{
    *this = other; 
}

Client::~Client()
{
    if (postFile.is_open())
        postFile.close();
    if (resFile.is_open())
        resFile.close();
}