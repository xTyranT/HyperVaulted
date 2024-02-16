
#include "../includes/Client.hpp"

Client::Client()
{
    read = false;
    enf = false;
    sread = 0;
    flag = false;
    chunksize = 0;
    contentlength = 0;
}

Client & Client::operator= ( const Client & other )
{
    (void ) other;
    read = false;
    enf = false;
    sread = 0;
    flag = false;
    chunksize = 0;
    contentlength = 0;
    return *this;
}

Client::Client (const Client & other )
{
    *this = other; 
}