
#include "../includes/Client.hpp"

Client::Client()
{
    first = false;
    read = false;
    enf = false;
    sread = 0;
    flag = false;
}


Client & Client::operator= ( const Client & other )
{
    (void ) other;
    first = false;
    read = false;
    enf = false;
    sread = 0;
    flag = false;
    return *this;
}

Client::Client (const Client & other )
{
    *this = other; 
}