
#include "../includes/Client.hpp"

Client::Client()
{
    sfd = 0;
    fd = 0;
    chunksize = 0;
    contentlength = 0;
    sread = 0;
    start = 0;
    chunkedbuff = "";
    request = "";
    requestHeader = "";
    read = false;
    flag = false;
    enf = false;
    resred = false;
    cgi = Cgi();
}

Client & Client::operator= ( const Client & other )
{
    sfd = other.sfd;
    fd = other.fd;
    chunksize = other.chunksize;
    contentlength = other.contentlength;
    sread = other.sread;
    start = other.start;
    chunkedbuff = other.chunkedbuff;
    request = other.request;
    requestHeader = other.requestHeader;
    reqRes = other.reqRes;
    read = other.read;
    flag = other.flag;
    enf = other.enf;
    resred = other.resred;
    cgi = other.cgi;
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